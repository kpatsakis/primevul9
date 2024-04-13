gst_matroska_demux_parse_id (GstMatroskaDemux * demux, guint32 id,
    guint64 length, guint needed)
{
  GstEbmlRead ebml = { 0, };
  GstFlowReturn ret = GST_FLOW_OK;
  guint64 read;

  GST_LOG_OBJECT (demux, "Parsing Element id 0x%x, "
      "size %" G_GUINT64_FORMAT ", prefix %d", id, length, needed);

  /* if we plan to read and parse this element, we need prefix (id + length)
   * and the contents */
  /* mind about overflow wrap-around when dealing with undefined size */
  read = length;
  if (G_LIKELY (length != G_MAXUINT64))
    read += needed;

  switch (demux->common.state) {
    case GST_MATROSKA_READ_STATE_START:
      switch (id) {
        case GST_EBML_ID_HEADER:
          GST_READ_CHECK (gst_matroska_demux_take (demux, read, &ebml));
          ret = gst_matroska_read_common_parse_header (&demux->common, &ebml);
          if (ret != GST_FLOW_OK)
            goto parse_failed;
          demux->common.state = GST_MATROSKA_READ_STATE_SEGMENT;
          gst_matroska_demux_check_seekability (demux);
          break;
        default:
          goto invalid_header;
          break;
      }
      break;
    case GST_MATROSKA_READ_STATE_SEGMENT:
      switch (id) {
        case GST_MATROSKA_ID_SEGMENT:
          /* eat segment prefix */
          GST_READ_CHECK (gst_matroska_demux_flush (demux, needed));
          GST_DEBUG_OBJECT (demux,
              "Found Segment start at offset %" G_GUINT64_FORMAT " with size %"
              G_GUINT64_FORMAT, demux->common.offset, length);
          /* seeks are from the beginning of the segment,
           * after the segment ID/length */
          demux->common.ebml_segment_start = demux->common.offset;
          if (length == 0)
            length = G_MAXUINT64;
          demux->common.ebml_segment_length = length;
          demux->common.state = GST_MATROSKA_READ_STATE_HEADER;
          break;
        default:
          GST_WARNING_OBJECT (demux,
              "Expected a Segment ID (0x%x), but received 0x%x!",
              GST_MATROSKA_ID_SEGMENT, id);
          GST_READ_CHECK (gst_matroska_demux_flush (demux, read));
          break;
      }
      break;
    case GST_MATROSKA_READ_STATE_SCANNING:
      if (id != GST_MATROSKA_ID_CLUSTER &&
          id != GST_MATROSKA_ID_PREVSIZE &&
          id != GST_MATROSKA_ID_CLUSTERTIMECODE) {
        if (demux->common.start_resync_offset != -1) {
          /* we need to skip byte per byte if we are scanning for a new cluster
           * after invalid data is found
           */
          read = 1;
        }
        goto skip;
      } else {
        if (demux->common.start_resync_offset != -1) {
          GST_LOG_OBJECT (demux, "Resync done, new cluster found!");
          demux->common.start_resync_offset = -1;
          demux->common.state = demux->common.state_to_restore;
        }
      }
      /* fall-through */
    case GST_MATROSKA_READ_STATE_HEADER:
    case GST_MATROSKA_READ_STATE_DATA:
    case GST_MATROSKA_READ_STATE_SEEK:
      switch (id) {
        case GST_EBML_ID_HEADER:
          GST_READ_CHECK (gst_matroska_demux_flush (demux, read));
          demux->common.state = GST_MATROSKA_READ_STATE_SEGMENT;
          gst_matroska_demux_check_seekability (demux);
          break;
        case GST_MATROSKA_ID_SEGMENTINFO:
          if (!demux->common.segmentinfo_parsed) {
            GST_READ_CHECK (gst_matroska_demux_take (demux, read, &ebml));
            ret = gst_matroska_read_common_parse_info (&demux->common,
                GST_ELEMENT_CAST (demux), &ebml);
            if (ret == GST_FLOW_OK)
              gst_matroska_demux_send_tags (demux);
          } else {
            GST_READ_CHECK (gst_matroska_demux_flush (demux, read));
          }
          break;
        case GST_MATROSKA_ID_TRACKS:
          GST_READ_CHECK (gst_matroska_demux_take (demux, read, &ebml));
          if (!demux->tracks_parsed) {
            ret = gst_matroska_demux_parse_tracks (demux, &ebml);
          } else {
            ret = gst_matroska_demux_update_tracks (demux, &ebml);
          }
          break;
        case GST_MATROSKA_ID_CLUSTER:
          if (G_UNLIKELY (!demux->tracks_parsed)) {
            if (demux->streaming) {
              GST_DEBUG_OBJECT (demux, "Cluster before Track");
              goto not_streamable;
            } else {
              ret = gst_matroska_demux_find_tracks (demux);
              if (!demux->tracks_parsed)
                goto no_tracks;
            }
          }
          if (demux->common.state == GST_MATROSKA_READ_STATE_HEADER) {
            demux->common.state = GST_MATROSKA_READ_STATE_DATA;
            demux->first_cluster_offset = demux->common.offset;

            if (!demux->streaming &&
                !GST_CLOCK_TIME_IS_VALID (demux->common.segment.duration)) {
              GstMatroskaIndex *last = NULL;

              GST_DEBUG_OBJECT (demux,
                  "estimating duration using last cluster");
              if ((last = gst_matroska_demux_search_pos (demux,
                          GST_CLOCK_TIME_NONE)) != NULL) {
                demux->last_cluster_offset =
                    last->pos + demux->common.ebml_segment_start;
                demux->stream_last_time = last->time;
                demux->common.segment.duration =
                    demux->stream_last_time - demux->stream_start_time;
                /* above estimate should not be taken all too strongly */
                demux->invalid_duration = TRUE;
                GST_DEBUG_OBJECT (demux,
                    "estimated duration as %" GST_TIME_FORMAT,
                    GST_TIME_ARGS (demux->common.segment.duration));

                g_free (last);
              }
            }

            /* Peek at second cluster in order to figure out if we have cluster
             * prev_size or not (which is never set on the first cluster for
             * obvious reasons). This is useful in case someone initiates a
             * seek or direction change before we reach the second cluster. */
            if (!demux->streaming) {
              ClusterInfo cluster = { 0, };

              if (gst_matroska_demux_peek_cluster_info (demux, &cluster,
                      demux->first_cluster_offset) && cluster.size > 0) {
                gst_matroska_demux_peek_cluster_info (demux, &cluster,
                    demux->first_cluster_offset + cluster.size);
              }
              demux->common.offset = demux->first_cluster_offset;
            }

            if (demux->deferred_seek_event) {
              GstEvent *seek_event;
              GstPad *seek_pad;
              seek_event = demux->deferred_seek_event;
              seek_pad = demux->deferred_seek_pad;
              demux->deferred_seek_event = NULL;
              demux->deferred_seek_pad = NULL;
              GST_DEBUG_OBJECT (demux,
                  "Handling deferred seek event: %" GST_PTR_FORMAT, seek_event);
              gst_matroska_demux_handle_seek_event (demux, seek_pad,
                  seek_event);
              gst_event_unref (seek_event);
            }

            /* send initial segment - we wait till we know the first
               incoming timestamp, so we can properly set the start of
               the segment. */
            demux->need_segment = TRUE;
          }
          demux->cluster_time = GST_CLOCK_TIME_NONE;
          demux->cluster_offset = demux->common.offset;
          demux->cluster_prevsize = 0;
          if (G_UNLIKELY (!demux->seek_first && demux->seek_block)) {
            GST_DEBUG_OBJECT (demux, "seek target block %" G_GUINT64_FORMAT
                " not found in Cluster, trying next Cluster's first block instead",
                demux->seek_block);
            demux->seek_block = 0;
          }
          demux->seek_first = FALSE;
          /* record next cluster for recovery */
          if (read != G_MAXUINT64)
            demux->next_cluster_offset = demux->cluster_offset + read;
          /* eat cluster prefix */
          gst_matroska_demux_flush (demux, needed);
          break;
        case GST_MATROSKA_ID_CLUSTERTIMECODE:
        {
          guint64 num;

          GST_READ_CHECK (gst_matroska_demux_take (demux, read, &ebml));
          if ((ret = gst_ebml_read_uint (&ebml, &id, &num)) != GST_FLOW_OK)
            goto parse_failed;
          GST_DEBUG_OBJECT (demux, "ClusterTimeCode: %" G_GUINT64_FORMAT, num);
          demux->cluster_time = num;
          /* track last cluster */
          if (demux->cluster_offset > demux->last_cluster_offset) {
            demux->last_cluster_offset = demux->cluster_offset;
            demux->stream_last_time =
                demux->cluster_time * demux->common.time_scale;
          }
#if 0
          if (demux->common.element_index) {
            if (demux->common.element_index_writer_id == -1)
              gst_index_get_writer_id (demux->common.element_index,
                  GST_OBJECT (demux), &demux->common.element_index_writer_id);
            GST_LOG_OBJECT (demux, "adding association %" GST_TIME_FORMAT "-> %"
                G_GUINT64_FORMAT " for writer id %d",
                GST_TIME_ARGS (demux->cluster_time), demux->cluster_offset,
                demux->common.element_index_writer_id);
            gst_index_add_association (demux->common.element_index,
                demux->common.element_index_writer_id,
                GST_ASSOCIATION_FLAG_KEY_UNIT,
                GST_FORMAT_TIME, demux->cluster_time,
                GST_FORMAT_BYTES, demux->cluster_offset, NULL);
          }
#endif
          break;
        }
        case GST_MATROSKA_ID_BLOCKGROUP:
          if (!gst_matroska_demux_seek_block (demux))
            goto skip;
          GST_READ_CHECK (gst_matroska_demux_take (demux, read, &ebml));
          DEBUG_ELEMENT_START (demux, &ebml, "BlockGroup");
          if ((ret = gst_ebml_read_master (&ebml, &id)) == GST_FLOW_OK) {
            ret = gst_matroska_demux_parse_blockgroup_or_simpleblock (demux,
                &ebml, demux->cluster_time, demux->cluster_offset, FALSE);
          }
          DEBUG_ELEMENT_STOP (demux, &ebml, "BlockGroup", ret);
          break;
        case GST_MATROSKA_ID_SIMPLEBLOCK:
          if (!gst_matroska_demux_seek_block (demux))
            goto skip;
          GST_READ_CHECK (gst_matroska_demux_take (demux, read, &ebml));
          DEBUG_ELEMENT_START (demux, &ebml, "SimpleBlock");
          ret = gst_matroska_demux_parse_blockgroup_or_simpleblock (demux,
              &ebml, demux->cluster_time, demux->cluster_offset, TRUE);
          DEBUG_ELEMENT_STOP (demux, &ebml, "SimpleBlock", ret);
          break;
        case GST_MATROSKA_ID_ATTACHMENTS:
          if (!demux->common.attachments_parsed) {
            GST_READ_CHECK (gst_matroska_demux_take (demux, read, &ebml));
            ret = gst_matroska_read_common_parse_attachments (&demux->common,
                GST_ELEMENT_CAST (demux), &ebml);
            if (ret == GST_FLOW_OK)
              gst_matroska_demux_send_tags (demux);
          } else {
            GST_READ_CHECK (gst_matroska_demux_flush (demux, read));
          }
          break;
        case GST_MATROSKA_ID_TAGS:
          GST_READ_CHECK (gst_matroska_demux_take (demux, read, &ebml));
          ret = gst_matroska_read_common_parse_metadata (&demux->common,
              GST_ELEMENT_CAST (demux), &ebml);
          if (ret == GST_FLOW_OK)
            gst_matroska_demux_send_tags (demux);
          break;
        case GST_MATROSKA_ID_CHAPTERS:
          if (!demux->common.chapters_parsed) {
            GST_READ_CHECK (gst_matroska_demux_take (demux, read, &ebml));
            ret =
                gst_matroska_read_common_parse_chapters (&demux->common, &ebml);

            if (demux->common.toc) {
              gst_matroska_demux_send_event (demux,
                  gst_event_new_toc (demux->common.toc, FALSE));
            }
          } else
            GST_READ_CHECK (gst_matroska_demux_flush (demux, read));
          break;
        case GST_MATROSKA_ID_SEEKHEAD:
          GST_READ_CHECK (gst_matroska_demux_take (demux, read, &ebml));
          ret = gst_matroska_demux_parse_contents (demux, &ebml);
          break;
        case GST_MATROSKA_ID_CUES:
          if (demux->common.index_parsed) {
            GST_READ_CHECK (gst_matroska_demux_flush (demux, read));
            break;
          }
          GST_READ_CHECK (gst_matroska_demux_take (demux, read, &ebml));
          ret = gst_matroska_read_common_parse_index (&demux->common, &ebml);
          /* only push based; delayed index building */
          if (ret == GST_FLOW_OK
              && demux->common.state == GST_MATROSKA_READ_STATE_SEEK) {
            GstEvent *event;

            GST_OBJECT_LOCK (demux);
            event = demux->seek_event;
            demux->seek_event = NULL;
            GST_OBJECT_UNLOCK (demux);

            g_assert (event);
            /* unlikely to fail, since we managed to seek to this point */
            if (!gst_matroska_demux_handle_seek_event (demux, NULL, event)) {
              gst_event_unref (event);
              goto seek_failed;
            }
            gst_event_unref (event);
            /* resume data handling, main thread clear to seek again */
            GST_OBJECT_LOCK (demux);
            demux->common.state = GST_MATROSKA_READ_STATE_DATA;
            GST_OBJECT_UNLOCK (demux);
          }
          break;
        case GST_MATROSKA_ID_PREVSIZE:{
          guint64 num;

          GST_READ_CHECK (gst_matroska_demux_take (demux, read, &ebml));
          if ((ret = gst_ebml_read_uint (&ebml, &id, &num)) != GST_FLOW_OK)
            goto parse_failed;
          GST_LOG_OBJECT (demux, "ClusterPrevSize: %" G_GUINT64_FORMAT, num);
          demux->cluster_prevsize = num;
          demux->seen_cluster_prevsize = TRUE;
          break;
        }
        case GST_MATROSKA_ID_POSITION:
        case GST_MATROSKA_ID_ENCRYPTEDBLOCK:
          /* The WebM doesn't support the EncryptedBlock element.
           * The Matroska spec doesn't give us more detail, how to parse this element,
           * for example the field TransformID isn't specified yet.*/
        case GST_MATROSKA_ID_SILENTTRACKS:
          GST_DEBUG_OBJECT (demux,
              "Skipping Cluster subelement 0x%x - ignoring", id);
          /* fall-through */
        default:
        skip:
          GST_DEBUG_OBJECT (demux, "skipping Element 0x%x", id);
          GST_READ_CHECK (gst_matroska_demux_flush (demux, read));
          break;
      }
      break;
  }

  if (ret == GST_FLOW_PARSE)
    goto parse_failed;

exit:
  gst_ebml_read_clear (&ebml);
  return ret;

  /* ERRORS */
read_error:
  {
    /* simply exit, maybe not enough data yet */
    /* no ebml to clear if read error */
    return ret;
  }
parse_failed:
  {
    GST_ELEMENT_ERROR (demux, STREAM, DEMUX, (NULL),
        ("Failed to parse Element 0x%x", id));
    ret = GST_FLOW_ERROR;
    goto exit;
  }
not_streamable:
  {
    GST_ELEMENT_ERROR (demux, STREAM, DEMUX, (NULL),
        ("File layout does not permit streaming"));
    ret = GST_FLOW_ERROR;
    goto exit;
  }
no_tracks:
  {
    GST_ELEMENT_ERROR (demux, STREAM, DEMUX, (NULL),
        ("No Tracks element found"));
    ret = GST_FLOW_ERROR;
    goto exit;
  }
invalid_header:
  {
    GST_ELEMENT_ERROR (demux, STREAM, DEMUX, (NULL), ("Invalid header"));
    ret = GST_FLOW_ERROR;
    goto exit;
  }
seek_failed:
  {
    GST_ELEMENT_ERROR (demux, STREAM, DEMUX, (NULL), ("Failed to seek"));
    ret = GST_FLOW_ERROR;
    goto exit;
  }
}