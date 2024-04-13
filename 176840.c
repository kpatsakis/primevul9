gst_matroska_demux_handle_src_event (GstPad * pad, GstObject * parent,
    GstEvent * event)
{
  GstMatroskaDemux *demux = GST_MATROSKA_DEMUX (parent);
  gboolean res = TRUE;

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_SEEK:
      /* no seeking until we are (safely) ready */
      if (demux->common.state != GST_MATROSKA_READ_STATE_DATA) {
        GST_DEBUG_OBJECT (demux,
            "not ready for seeking yet, deferring seek event: %" GST_PTR_FORMAT,
            event);
        if (demux->deferred_seek_event)
          gst_event_unref (demux->deferred_seek_event);
        demux->deferred_seek_event = event;
        demux->deferred_seek_pad = pad;
        return TRUE;
      }

      {
        guint32 seqnum = gst_event_get_seqnum (event);
        if (seqnum == demux->segment_seqnum) {
          GST_LOG_OBJECT (pad,
              "Drop duplicated SEEK event seqnum %" G_GUINT32_FORMAT, seqnum);
          gst_event_unref (event);
          return TRUE;
        }
      }

      if (!demux->streaming)
        res = gst_matroska_demux_handle_seek_event (demux, pad, event);
      else
        res = gst_matroska_demux_handle_seek_push (demux, pad, event);
      gst_event_unref (event);
      break;

    case GST_EVENT_QOS:
    {
      GstMatroskaTrackContext *context = gst_pad_get_element_private (pad);
      if (context->type == GST_MATROSKA_TRACK_TYPE_VIDEO) {
        GstMatroskaTrackVideoContext *videocontext =
            (GstMatroskaTrackVideoContext *) context;
        gdouble proportion;
        GstClockTimeDiff diff;
        GstClockTime timestamp;

        gst_event_parse_qos (event, NULL, &proportion, &diff, &timestamp);

        GST_OBJECT_LOCK (demux);
        videocontext->earliest_time = timestamp + diff;
        GST_OBJECT_UNLOCK (demux);
      }
      res = TRUE;
      gst_event_unref (event);
      break;
    }

    case GST_EVENT_TOC_SELECT:
    {
      char *uid = NULL;
      GstTocEntry *entry = NULL;
      GstEvent *seek_event;
      gint64 start_pos;

      if (!demux->common.toc) {
        GST_DEBUG_OBJECT (demux, "no TOC to select");
        return FALSE;
      } else {
        gst_event_parse_toc_select (event, &uid);
        if (uid != NULL) {
          GST_OBJECT_LOCK (demux);
          entry = gst_toc_find_entry (demux->common.toc, uid);
          if (entry == NULL) {
            GST_OBJECT_UNLOCK (demux);
            GST_WARNING_OBJECT (demux, "no TOC entry with given UID: %s", uid);
            res = FALSE;
          } else {
            gst_toc_entry_get_start_stop_times (entry, &start_pos, NULL);
            GST_OBJECT_UNLOCK (demux);
            seek_event = gst_event_new_seek (1.0,
                GST_FORMAT_TIME,
                GST_SEEK_FLAG_FLUSH,
                GST_SEEK_TYPE_SET, start_pos, GST_SEEK_TYPE_SET, -1);
            res = gst_matroska_demux_handle_seek_event (demux, pad, seek_event);
            gst_event_unref (seek_event);
          }
          g_free (uid);
        } else {
          GST_WARNING_OBJECT (demux, "received empty TOC select event");
          res = FALSE;
        }
      }
      gst_event_unref (event);
      break;
    }

      /* events we don't need to handle */
    case GST_EVENT_NAVIGATION:
      gst_event_unref (event);
      res = FALSE;
      break;

    case GST_EVENT_LATENCY:
    default:
      res = gst_pad_push_event (demux->common.sinkpad, event);
      break;
  }

  return res;
}