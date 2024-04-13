gst_qtdemux_handle_sink_event (GstPad * sinkpad, GstObject * parent,
    GstEvent * event)
{
  GstQTDemux *demux = GST_QTDEMUX (parent);
  gboolean res = TRUE;

  GST_LOG_OBJECT (demux, "handling %s event", GST_EVENT_TYPE_NAME (event));

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_SEGMENT:
    {
      gint64 offset = 0;
      QtDemuxStream *stream;
      gint idx;
      GstSegment segment;

      /* some debug output */
      gst_event_copy_segment (event, &segment);
      GST_DEBUG_OBJECT (demux, "received newsegment %" GST_SEGMENT_FORMAT,
          &segment);

      /* erase any previously set segment */
      gst_event_replace (&demux->pending_newsegment, NULL);

      if (segment.format == GST_FORMAT_TIME) {
        GST_DEBUG_OBJECT (demux, "new pending_newsegment");
        gst_event_replace (&demux->pending_newsegment, event);
        demux->upstream_format_is_time = TRUE;
      } else {
        GST_DEBUG_OBJECT (demux, "Not storing upstream newsegment, "
            "not in time format");

        /* chain will send initial newsegment after pads have been added */
        if (demux->state != QTDEMUX_STATE_MOVIE || !demux->n_streams) {
          GST_DEBUG_OBJECT (demux, "still starting, eating event");
          goto exit;
        }
      }

      /* check if this matches a time seek we received previously
       * FIXME for backwards compatibility reasons we use the
       * seek_offset here to compare. In the future we might want to
       * change this to use the seqnum as it uniquely should identify
       * the segment that corresponds to the seek. */
      GST_DEBUG_OBJECT (demux, "Stored seek offset: %" G_GINT64_FORMAT
          ", received segment offset %" G_GINT64_FORMAT,
          demux->seek_offset, segment.start);
      if (segment.format == GST_FORMAT_BYTES
          && demux->seek_offset == segment.start) {
        GST_OBJECT_LOCK (demux);
        offset = segment.start;

        segment.format = GST_FORMAT_TIME;
        segment.start = demux->push_seek_start;
        segment.stop = demux->push_seek_stop;
        GST_DEBUG_OBJECT (demux, "Replaced segment with stored seek "
            "segment %" GST_TIME_FORMAT " - %" GST_TIME_FORMAT,
            GST_TIME_ARGS (segment.start), GST_TIME_ARGS (segment.stop));
        GST_OBJECT_UNLOCK (demux);
      }

      /* we only expect a BYTE segment, e.g. following a seek */
      if (segment.format == GST_FORMAT_BYTES) {
        if (GST_CLOCK_TIME_IS_VALID (segment.start)) {
          offset = segment.start;

          gst_qtdemux_find_sample (demux, segment.start, TRUE, FALSE, NULL,
              NULL, (gint64 *) & segment.start);
          if ((gint64) segment.start < 0)
            segment.start = 0;
        }
        if (GST_CLOCK_TIME_IS_VALID (segment.stop)) {
          gst_qtdemux_find_sample (demux, segment.stop, FALSE, FALSE, NULL,
              NULL, (gint64 *) & segment.stop);
          /* keyframe seeking should already arrange for start >= stop,
           * but make sure in other rare cases */
          segment.stop = MAX (segment.stop, segment.start);
        }
      } else if (segment.format == GST_FORMAT_TIME) {
        /* push all data on the adapter before starting this
         * new segment */
        gst_qtdemux_process_adapter (demux, TRUE);
      } else {
        GST_DEBUG_OBJECT (demux, "unsupported segment format, ignoring");
        goto exit;
      }

      /* We shouldn't modify upstream driven TIME FORMAT segment */
      if (!demux->upstream_format_is_time) {
        /* accept upstream's notion of segment and distribute along */
        segment.format = GST_FORMAT_TIME;
        segment.position = segment.time = segment.start;
        segment.duration = demux->segment.duration;
        segment.base = gst_segment_to_running_time (&demux->segment,
            GST_FORMAT_TIME, demux->segment.position);
      }

      gst_segment_copy_into (&segment, &demux->segment);
      GST_DEBUG_OBJECT (demux, "Pushing newseg %" GST_SEGMENT_FORMAT, &segment);

      /* map segment to internal qt segments and push on each stream */
      if (demux->n_streams) {
        if (demux->fragmented) {
          GstEvent *segment_event = gst_event_new_segment (&segment);

          gst_event_replace (&demux->pending_newsegment, NULL);
          gst_event_set_seqnum (segment_event, demux->segment_seqnum);
          gst_qtdemux_push_event (demux, segment_event);
        } else {
          gst_event_replace (&demux->pending_newsegment, NULL);
          gst_qtdemux_map_and_push_segments (demux, &segment);
        }
      }

      /* clear leftover in current segment, if any */
      gst_adapter_clear (demux->adapter);

      /* set up streaming thread */
      demux->offset = offset;
      if (demux->upstream_format_is_time) {
        GST_DEBUG_OBJECT (demux, "Upstream is driving in time format, "
            "set values to restart reading from a new atom");
        demux->neededbytes = 16;
        demux->todrop = 0;
      } else {
        gst_qtdemux_find_sample (demux, offset, TRUE, TRUE, &stream, &idx,
            NULL);
        if (stream) {
          demux->todrop = stream->samples[idx].offset - offset;
          demux->neededbytes = demux->todrop + stream->samples[idx].size;
        } else {
          /* set up for EOS */
          demux->neededbytes = -1;
          demux->todrop = 0;
        }
      }
    exit:
      gst_event_unref (event);
      res = TRUE;
      goto drop;
    }
    case GST_EVENT_FLUSH_START:
    {
      if (gst_event_get_seqnum (event) == demux->offset_seek_seqnum) {
        gst_event_unref (event);
        goto drop;
      }
      break;
    }
    case GST_EVENT_FLUSH_STOP:
    {
      guint64 dur;

      dur = demux->segment.duration;
      gst_qtdemux_reset (demux, FALSE);
      demux->segment.duration = dur;

      if (gst_event_get_seqnum (event) == demux->offset_seek_seqnum) {
        gst_event_unref (event);
        goto drop;
      }
      break;
    }
    case GST_EVENT_EOS:
      /* If we are in push mode, and get an EOS before we've seen any streams,
       * then error out - we have nowhere to send the EOS */
      if (!demux->pullbased) {
        gint i;
        gboolean has_valid_stream = FALSE;
        for (i = 0; i < demux->n_streams; i++) {
          if (demux->streams[i]->pad != NULL) {
            has_valid_stream = TRUE;
            break;
          }
        }
        if (!has_valid_stream)
          gst_qtdemux_post_no_playable_stream_error (demux);
        else {
          GST_DEBUG_OBJECT (demux, "Data still available after EOS: %u",
              (guint) gst_adapter_available (demux->adapter));
          if (gst_qtdemux_process_adapter (demux, TRUE) != GST_FLOW_OK) {
            res = FALSE;
          }
        }
      }
      break;
    case GST_EVENT_CAPS:{
      GstCaps *caps = NULL;

      gst_event_parse_caps (event, &caps);
      gst_qtdemux_setcaps (demux, caps);
      res = TRUE;
      gst_event_unref (event);
      goto drop;
    }
    case GST_EVENT_PROTECTION:
    {
      const gchar *system_id = NULL;

      gst_event_parse_protection (event, &system_id, NULL, NULL);
      GST_DEBUG_OBJECT (demux, "Received protection event for system ID %s",
          system_id);
      gst_qtdemux_append_protection_system_id (demux, system_id);
      /* save the event for later, for source pads that have not been created */
      g_queue_push_tail (&demux->protection_event_queue, gst_event_ref (event));
      /* send it to all pads that already exist */
      gst_qtdemux_push_event (demux, event);
      res = TRUE;
      goto drop;
    }
    default:
      break;
  }

  res = gst_pad_event_default (demux->sinkpad, parent, event) & res;

drop:
  return res;
}