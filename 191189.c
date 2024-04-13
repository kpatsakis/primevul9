gst_qtdemux_process_adapter (GstQTDemux * demux, gboolean force)
{
  GstFlowReturn ret = GST_FLOW_OK;

  /* we never really mean to buffer that much */
  if (demux->neededbytes == -1) {
    goto eos;
  }

  while (((gst_adapter_available (demux->adapter)) >= demux->neededbytes) &&
      (ret == GST_FLOW_OK || (ret == GST_FLOW_NOT_LINKED && force))) {

#ifndef GST_DISABLE_GST_DEBUG
    {
      guint64 discont_offset, distance_from_discont;

      discont_offset = gst_adapter_offset_at_discont (demux->adapter);
      distance_from_discont =
          gst_adapter_distance_from_discont (demux->adapter);

      GST_DEBUG_OBJECT (demux,
          "state:%s , demux->neededbytes:%d, demux->offset:%" G_GUINT64_FORMAT
          " adapter offset :%" G_GUINT64_FORMAT " (+ %" G_GUINT64_FORMAT
          " bytes)", qt_demux_state_string (demux->state), demux->neededbytes,
          demux->offset, discont_offset, distance_from_discont);
    }
#endif

    switch (demux->state) {
      case QTDEMUX_STATE_INITIAL:{
        const guint8 *data;
        guint32 fourcc;
        guint64 size;

        gst_qtdemux_check_seekability (demux);

        data = gst_adapter_map (demux->adapter, demux->neededbytes);

        /* get fourcc/length, set neededbytes */
        extract_initial_length_and_fourcc ((guint8 *) data, demux->neededbytes,
            &size, &fourcc);
        gst_adapter_unmap (demux->adapter);
        data = NULL;
        GST_DEBUG_OBJECT (demux, "Peeking found [%" GST_FOURCC_FORMAT "] "
            "size: %" G_GUINT64_FORMAT, GST_FOURCC_ARGS (fourcc), size);
        if (size == 0) {
          GST_ELEMENT_ERROR (demux, STREAM, DEMUX,
              (_("This file is invalid and cannot be played.")),
              ("initial atom '%" GST_FOURCC_FORMAT "' has empty length",
                  GST_FOURCC_ARGS (fourcc)));
          ret = GST_FLOW_ERROR;
          break;
        }
        if (fourcc == FOURCC_mdat) {
          gint next_entry = next_entry_size (demux);
          if (demux->n_streams > 0 && (next_entry != -1 || !demux->fragmented)) {
            /* we have the headers, start playback */
            demux->state = QTDEMUX_STATE_MOVIE;
            demux->neededbytes = next_entry;
            demux->mdatleft = size;
          } else {
            /* no headers yet, try to get them */
            guint bs;
            gboolean res;
            guint64 old, target;

          buffer_data:
            old = demux->offset;
            target = old + size;

            /* try to jump over the atom with a seek */
            /* only bother if it seems worth doing so,
             * and avoids possible upstream/server problems */
            if (demux->upstream_seekable &&
                demux->upstream_size > 4 * (1 << 20)) {
              res = qtdemux_seek_offset (demux, target);
            } else {
              GST_DEBUG_OBJECT (demux, "skipping seek");
              res = FALSE;
            }

            if (res) {
              GST_DEBUG_OBJECT (demux, "seek success");
              /* remember the offset fo the first mdat so we can seek back to it
               * after we have the headers */
              if (fourcc == FOURCC_mdat && demux->first_mdat == -1) {
                demux->first_mdat = old;
                GST_DEBUG_OBJECT (demux, "first mdat at %" G_GUINT64_FORMAT,
                    demux->first_mdat);
              }
              /* seek worked, continue reading */
              demux->offset = target;
              demux->neededbytes = 16;
              demux->state = QTDEMUX_STATE_INITIAL;
            } else {
              /* seek failed, need to buffer */
              demux->offset = old;
              GST_DEBUG_OBJECT (demux, "seek failed/skipped");
              /* there may be multiple mdat (or alike) buffers */
              /* sanity check */
              if (demux->mdatbuffer)
                bs = gst_buffer_get_size (demux->mdatbuffer);
              else
                bs = 0;
              if (size + bs > 10 * (1 << 20))
                goto no_moov;
              demux->state = QTDEMUX_STATE_BUFFER_MDAT;
              demux->neededbytes = size;
              if (!demux->mdatbuffer)
                demux->mdatoffset = demux->offset;
            }
          }
        } else if (G_UNLIKELY (size > QTDEMUX_MAX_ATOM_SIZE)) {
          GST_ELEMENT_ERROR (demux, STREAM, DEMUX,
              (_("This file is invalid and cannot be played.")),
              ("atom %" GST_FOURCC_FORMAT " has bogus size %" G_GUINT64_FORMAT,
                  GST_FOURCC_ARGS (fourcc), size));
          ret = GST_FLOW_ERROR;
          break;
        } else {
          /* this means we already started buffering and still no moov header,
           * let's continue buffering everything till we get moov */
          if (demux->mdatbuffer && !(fourcc == FOURCC_moov
                  || fourcc == FOURCC_moof))
            goto buffer_data;
          demux->neededbytes = size;
          demux->state = QTDEMUX_STATE_HEADER;
        }
        break;
      }
      case QTDEMUX_STATE_HEADER:{
        const guint8 *data;
        guint32 fourcc;

        GST_DEBUG_OBJECT (demux, "In header");

        data = gst_adapter_map (demux->adapter, demux->neededbytes);

        /* parse the header */
        extract_initial_length_and_fourcc (data, demux->neededbytes, NULL,
            &fourcc);
        if (fourcc == FOURCC_moov) {
          gint n;

          /* in usual fragmented setup we could try to scan for more
           * and end up at the the moov (after mdat) again */
          if (demux->got_moov && demux->n_streams > 0 &&
              (!demux->fragmented
                  || demux->last_moov_offset == demux->offset)) {
            GST_DEBUG_OBJECT (demux,
                "Skipping moov atom as we have (this) one already");
          } else {
            GST_DEBUG_OBJECT (demux, "Parsing [moov]");

            if (demux->got_moov && demux->fragmented) {
              GST_DEBUG_OBJECT (demux,
                  "Got a second moov, clean up data from old one");
              if (demux->moov_node)
                g_node_destroy (demux->moov_node);
              demux->moov_node = NULL;
              demux->moov_node_compressed = NULL;
            } else {
              /* prepare newsegment to send when streaming actually starts */
              if (!demux->pending_newsegment) {
                demux->pending_newsegment =
                    gst_event_new_segment (&demux->segment);
                if (demux->segment_seqnum)
                  gst_event_set_seqnum (demux->pending_newsegment,
                      demux->segment_seqnum);
              }
            }

            demux->last_moov_offset = demux->offset;

            qtdemux_parse_moov (demux, data, demux->neededbytes);
            qtdemux_node_dump (demux, demux->moov_node);
            qtdemux_parse_tree (demux);
            qtdemux_prepare_streams (demux);
            if (!demux->got_moov)
              qtdemux_expose_streams (demux);
            else {

              for (n = 0; n < demux->n_streams; n++) {
                QtDemuxStream *stream = demux->streams[n];

                gst_qtdemux_configure_stream (demux, stream);
              }
            }

            demux->got_moov = TRUE;
            gst_qtdemux_check_send_pending_segment (demux);

            /* fragmented streams headers shouldn't contain edts atoms */
            if (!demux->fragmented) {
              for (n = 0; n < demux->n_streams; n++) {
                gst_qtdemux_stream_send_initial_gap_segments (demux,
                    demux->streams[n]);
              }
            }

            g_node_destroy (demux->moov_node);
            demux->moov_node = NULL;
            GST_DEBUG_OBJECT (demux, "Finished parsing the header");
          }
        } else if (fourcc == FOURCC_moof) {
          if ((demux->got_moov || demux->media_caps) && demux->fragmented) {
            guint64 dist = 0;
            GstClockTime prev_pts;
            guint64 prev_offset;
            guint64 adapter_discont_offset, adapter_discont_dist;

            GST_DEBUG_OBJECT (demux, "Parsing [moof]");

            /*
             * The timestamp of the moof buffer is relevant as some scenarios
             * won't have the initial timestamp in the atoms. Whenever a new
             * buffer has started, we get that buffer's PTS and use it as a base
             * timestamp for the trun entries.
             *
             * To keep track of the current buffer timestamp and starting point
             * we use gst_adapter_prev_pts that gives us the PTS and the distance
             * from the beggining of the buffer, with the distance and demux->offset
             * we know if it is still the same buffer or not.
             */
            prev_pts = gst_adapter_prev_pts (demux->adapter, &dist);
            prev_offset = demux->offset - dist;
            if (demux->fragment_start_offset == -1
                || prev_offset > demux->fragment_start_offset) {
              demux->fragment_start_offset = prev_offset;
              demux->fragment_start = prev_pts;
              GST_DEBUG_OBJECT (demux,
                  "New fragment start found at: %" G_GUINT64_FORMAT " : %"
                  GST_TIME_FORMAT, demux->fragment_start_offset,
                  GST_TIME_ARGS (demux->fragment_start));
            }

            /* We can't use prev_offset() here because this would require
             * upstream to set consistent and correct offsets on all buffers
             * since the discont. Nothing ever did that in the past and we
             * would break backwards compatibility here then.
             * Instead take the offset we had at the last discont and count
             * the bytes from there. This works with old code as there would
             * be no discont between moov and moof, and also works with
             * adaptivedemux which correctly sets offset and will set the
             * DISCONT flag accordingly when needed.
             *
             * We also only do this for upstream TIME segments as otherwise
             * there are potential backwards compatibility problems with
             * seeking in PUSH mode and upstream providing inconsistent
             * timestamps. */
            adapter_discont_offset =
                gst_adapter_offset_at_discont (demux->adapter);
            adapter_discont_dist =
                gst_adapter_distance_from_discont (demux->adapter);

            GST_DEBUG_OBJECT (demux,
                "demux offset %" G_GUINT64_FORMAT " adapter offset %"
                G_GUINT64_FORMAT " (+ %" G_GUINT64_FORMAT " bytes)",
                demux->offset, adapter_discont_offset, adapter_discont_dist);

            if (demux->upstream_format_is_time) {
              demux->moof_offset = adapter_discont_offset;
              if (demux->moof_offset != GST_BUFFER_OFFSET_NONE)
                demux->moof_offset += adapter_discont_dist;
              if (demux->moof_offset == GST_BUFFER_OFFSET_NONE)
                demux->moof_offset = demux->offset;
            } else {
              demux->moof_offset = demux->offset;
            }

            if (!qtdemux_parse_moof (demux, data, demux->neededbytes,
                    demux->moof_offset, NULL)) {
              gst_adapter_unmap (demux->adapter);
              ret = GST_FLOW_ERROR;
              goto done;
            }
            /* in MSS we need to expose the pads after the first moof as we won't get a moov */
            if (demux->mss_mode && !demux->exposed) {
              if (!demux->pending_newsegment) {
                GST_DEBUG_OBJECT (demux, "new pending_newsegment");
                demux->pending_newsegment =
                    gst_event_new_segment (&demux->segment);
                if (demux->segment_seqnum)
                  gst_event_set_seqnum (demux->pending_newsegment,
                      demux->segment_seqnum);
              }
              qtdemux_expose_streams (demux);
            }
          } else {
            GST_DEBUG_OBJECT (demux, "Discarding [moof]");
          }
        } else if (fourcc == FOURCC_ftyp) {
          GST_DEBUG_OBJECT (demux, "Parsing [ftyp]");
          qtdemux_parse_ftyp (demux, data, demux->neededbytes);
        } else if (fourcc == FOURCC_uuid) {
          GST_DEBUG_OBJECT (demux, "Parsing [uuid]");
          qtdemux_parse_uuid (demux, data, demux->neededbytes);
        } else if (fourcc == FOURCC_sidx) {
          GST_DEBUG_OBJECT (demux, "Parsing [sidx]");
          qtdemux_parse_sidx (demux, data, demux->neededbytes);
        } else {
          GST_WARNING_OBJECT (demux,
              "Unknown fourcc while parsing header : %" GST_FOURCC_FORMAT,
              GST_FOURCC_ARGS (fourcc));
          /* Let's jump that one and go back to initial state */
        }
        gst_adapter_unmap (demux->adapter);
        data = NULL;

        if (demux->mdatbuffer && demux->n_streams) {
          gsize remaining_data_size = 0;

          /* the mdat was before the header */
          GST_DEBUG_OBJECT (demux, "We have n_streams:%d and mdatbuffer:%p",
              demux->n_streams, demux->mdatbuffer);
          /* restore our adapter/offset view of things with upstream;
           * put preceding buffered data ahead of current moov data.
           * This should also handle evil mdat, moov, mdat cases and alike */
          gst_adapter_flush (demux->adapter, demux->neededbytes);

          /* Store any remaining data after the mdat for later usage */
          remaining_data_size = gst_adapter_available (demux->adapter);
          if (remaining_data_size > 0) {
            g_assert (demux->restoredata_buffer == NULL);
            demux->restoredata_buffer =
                gst_adapter_take_buffer (demux->adapter, remaining_data_size);
            demux->restoredata_offset = demux->offset + demux->neededbytes;
            GST_DEBUG_OBJECT (demux,
                "Stored %" G_GSIZE_FORMAT " post mdat bytes at offset %"
                G_GUINT64_FORMAT, remaining_data_size,
                demux->restoredata_offset);
          }

          gst_adapter_push (demux->adapter, demux->mdatbuffer);
          demux->mdatbuffer = NULL;
          demux->offset = demux->mdatoffset;
          demux->neededbytes = next_entry_size (demux);
          demux->state = QTDEMUX_STATE_MOVIE;
          demux->mdatleft = gst_adapter_available (demux->adapter);
        } else {
          GST_DEBUG_OBJECT (demux, "Carrying on normally");
          gst_adapter_flush (demux->adapter, demux->neededbytes);

          /* only go back to the mdat if there are samples to play */
          if (demux->got_moov && demux->first_mdat != -1
              && has_next_entry (demux)) {
            gboolean res;

            /* we need to seek back */
            res = qtdemux_seek_offset (demux, demux->first_mdat);
            if (res) {
              demux->offset = demux->first_mdat;
            } else {
              GST_DEBUG_OBJECT (demux, "Seek back failed");
            }
          } else {
            demux->offset += demux->neededbytes;
          }
          demux->neededbytes = 16;
          demux->state = QTDEMUX_STATE_INITIAL;
        }

        break;
      }
      case QTDEMUX_STATE_BUFFER_MDAT:{
        GstBuffer *buf;
        guint8 fourcc[4];

        GST_DEBUG_OBJECT (demux, "Got our buffer at offset %" G_GUINT64_FORMAT,
            demux->offset);
        buf = gst_adapter_take_buffer (demux->adapter, demux->neededbytes);
        gst_buffer_extract (buf, 0, fourcc, 4);
        GST_DEBUG_OBJECT (demux, "mdatbuffer starts with %" GST_FOURCC_FORMAT,
            GST_FOURCC_ARGS (QT_FOURCC (fourcc)));
        if (demux->mdatbuffer)
          demux->mdatbuffer = gst_buffer_append (demux->mdatbuffer, buf);
        else
          demux->mdatbuffer = buf;
        demux->offset += demux->neededbytes;
        demux->neededbytes = 16;
        demux->state = QTDEMUX_STATE_INITIAL;
        gst_qtdemux_post_progress (demux, 1, 1);

        break;
      }
      case QTDEMUX_STATE_MOVIE:{
        QtDemuxStream *stream = NULL;
        QtDemuxSample *sample;
        int i = -1;
        GstClockTime dts, pts, duration;
        gboolean keyframe;

        GST_DEBUG_OBJECT (demux,
            "BEGIN // in MOVIE for offset %" G_GUINT64_FORMAT, demux->offset);

        if (demux->fragmented) {
          GST_DEBUG_OBJECT (demux, "mdat remaining %" G_GUINT64_FORMAT,
              demux->mdatleft);
          if (G_LIKELY (demux->todrop < demux->mdatleft)) {
            /* if needed data starts within this atom,
             * then it should not exceed this atom */
            if (G_UNLIKELY (demux->neededbytes > demux->mdatleft)) {
              GST_ELEMENT_ERROR (demux, STREAM, DEMUX,
                  (_("This file is invalid and cannot be played.")),
                  ("sample data crosses atom boundary"));
              ret = GST_FLOW_ERROR;
              break;
            }
            demux->mdatleft -= demux->neededbytes;
          } else {
            GST_DEBUG_OBJECT (demux, "data atom emptied; resuming atom scan");
            /* so we are dropping more than left in this atom */
            gst_qtdemux_drop_data (demux, demux->mdatleft);
            demux->mdatleft = 0;

            /* need to resume atom parsing so we do not miss any other pieces */
            demux->state = QTDEMUX_STATE_INITIAL;
            demux->neededbytes = 16;

            /* check if there was any stored post mdat data from previous buffers */
            if (demux->restoredata_buffer) {
              g_assert (gst_adapter_available (demux->adapter) == 0);

              gst_adapter_push (demux->adapter, demux->restoredata_buffer);
              demux->restoredata_buffer = NULL;
              demux->offset = demux->restoredata_offset;
            }

            break;
          }
        }

        if (demux->todrop) {
          if (demux->cenc_aux_info_offset > 0) {
            GstByteReader br;
            const guint8 *data;

            GST_DEBUG_OBJECT (demux, "parsing cenc auxiliary info");
            data = gst_adapter_map (demux->adapter, demux->todrop);
            gst_byte_reader_init (&br, data + 8, demux->todrop);
            if (!qtdemux_parse_cenc_aux_info (demux, demux->streams[0], &br,
                    demux->cenc_aux_info_sizes, demux->cenc_aux_sample_count)) {
              GST_ERROR_OBJECT (demux, "failed to parse cenc auxiliary info");
              ret = GST_FLOW_ERROR;
              gst_adapter_unmap (demux->adapter);
              g_free (demux->cenc_aux_info_sizes);
              demux->cenc_aux_info_sizes = NULL;
              goto done;
            }
            demux->cenc_aux_info_offset = 0;
            g_free (demux->cenc_aux_info_sizes);
            demux->cenc_aux_info_sizes = NULL;
            gst_adapter_unmap (demux->adapter);
          }
          gst_qtdemux_drop_data (demux, demux->todrop);
        }

        /* first buffer? */
        /* initial newsegment sent here after having added pads,
         * possible others in sink_event */
        gst_qtdemux_check_send_pending_segment (demux);

        /* Figure out which stream this packet belongs to */
        for (i = 0; i < demux->n_streams; i++) {
          stream = demux->streams[i];
          if (stream->sample_index >= stream->n_samples)
            continue;
          GST_LOG_OBJECT (demux,
              "Checking stream %d (sample_index:%d / offset:%" G_GUINT64_FORMAT
              " / size:%d)", i, stream->sample_index,
              stream->samples[stream->sample_index].offset,
              stream->samples[stream->sample_index].size);

          if (stream->samples[stream->sample_index].offset == demux->offset)
            break;
        }

        if (G_UNLIKELY (stream == NULL || i == demux->n_streams))
          goto unknown_stream;

        if (stream->new_caps) {
          gst_qtdemux_configure_stream (demux, stream);
        }

        /* Put data in a buffer, set timestamps, caps, ... */
        sample = &stream->samples[stream->sample_index];

        if (G_LIKELY (!(STREAM_IS_EOS (stream)))) {
          GST_DEBUG_OBJECT (demux, "stream : %" GST_FOURCC_FORMAT,
              GST_FOURCC_ARGS (stream->fourcc));

          dts = QTSAMPLE_DTS (stream, sample);
          pts = QTSAMPLE_PTS (stream, sample);
          duration = QTSAMPLE_DUR_DTS (stream, sample, dts);
          keyframe = QTSAMPLE_KEYFRAME (stream, sample);

          /* check for segment end */
          if (G_UNLIKELY (demux->segment.stop != -1
                  && demux->segment.stop <= pts && stream->on_keyframe)) {
            GST_DEBUG_OBJECT (demux, "we reached the end of our segment.");
            stream->time_position = GST_CLOCK_TIME_NONE;        /* this means EOS */

            /* skip this data, stream is EOS */
            gst_adapter_flush (demux->adapter, demux->neededbytes);

            /* check if all streams are eos */
            ret = GST_FLOW_EOS;
            for (i = 0; i < demux->n_streams; i++) {
              if (!STREAM_IS_EOS (demux->streams[i])) {
                ret = GST_FLOW_OK;
                break;
              }
            }

            if (ret == GST_FLOW_EOS) {
              GST_DEBUG_OBJECT (demux, "All streams are EOS, signal upstream");
              goto eos;
            }
          } else {
            GstBuffer *outbuf;

            outbuf =
                gst_adapter_take_buffer (demux->adapter, demux->neededbytes);

            /* FIXME: should either be an assert or a plain check */
            g_return_val_if_fail (outbuf != NULL, GST_FLOW_ERROR);

            ret = gst_qtdemux_decorate_and_push_buffer (demux, stream, outbuf,
                dts, pts, duration, keyframe, dts, demux->offset);
          }

          /* combine flows */
          ret = gst_qtdemux_combine_flows (demux, stream, ret);
          if (ret != GST_FLOW_OK && ret != GST_FLOW_NOT_LINKED)
            goto non_ok_unlinked_flow;
        } else {
          /* skip this data, stream is EOS */
          gst_adapter_flush (demux->adapter, demux->neededbytes);
        }

        stream->sample_index++;
        stream->offset_in_sample = 0;

        /* update current offset and figure out size of next buffer */
        GST_LOG_OBJECT (demux, "increasing offset %" G_GUINT64_FORMAT " by %u",
            demux->offset, demux->neededbytes);
        demux->offset += demux->neededbytes;
        GST_LOG_OBJECT (demux, "offset is now %" G_GUINT64_FORMAT,
            demux->offset);

        if ((demux->neededbytes = next_entry_size (demux)) == -1) {
          if (demux->fragmented) {
            GST_DEBUG_OBJECT (demux, "(temporarily) out of fragmented samples");
            /* there may be more to follow, only finish this atom */
            demux->todrop = demux->mdatleft;
            demux->neededbytes = demux->todrop;
            break;
          }
          goto eos;
        }
        break;
      }
      default:
        goto invalid_state;
    }
  }

  /* when buffering movie data, at least show user something is happening */
  if (ret == GST_FLOW_OK && demux->state == QTDEMUX_STATE_BUFFER_MDAT &&
      gst_adapter_available (demux->adapter) <= demux->neededbytes) {
    gst_qtdemux_post_progress (demux, gst_adapter_available (demux->adapter),
        demux->neededbytes);
  }
done:

  return ret;

  /* ERRORS */
non_ok_unlinked_flow:
  {
    GST_DEBUG_OBJECT (demux, "Stopping, combined return flow %s",
        gst_flow_get_name (ret));
    return ret;
  }
unknown_stream:
  {
    GST_ELEMENT_ERROR (demux, STREAM, FAILED, (NULL), ("unknown stream found"));
    ret = GST_FLOW_ERROR;
    goto done;
  }
eos:
  {
    GST_DEBUG_OBJECT (demux, "no next entry, EOS");
    ret = GST_FLOW_EOS;
    goto done;
  }
invalid_state:
  {
    GST_ELEMENT_ERROR (demux, STREAM, FAILED,
        (NULL), ("qtdemuxer invalid state %d", demux->state));
    ret = GST_FLOW_ERROR;
    goto done;
  }
no_moov:
  {
    GST_ELEMENT_ERROR (demux, STREAM, FAILED,
        (NULL), ("no 'moov' atom within the first 10 MB"));
    ret = GST_FLOW_ERROR;
    goto done;
  }
}