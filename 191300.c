gst_qtdemux_handle_src_event (GstPad * pad, GstObject * parent,
    GstEvent * event)
{
  gboolean res = TRUE;
  GstQTDemux *qtdemux = GST_QTDEMUX (parent);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_SEEK:
    {
#ifndef GST_DISABLE_GST_DEBUG
      GstClockTime ts = gst_util_get_timestamp ();
#endif

      if (qtdemux->upstream_format_is_time && qtdemux->fragmented) {
        /* seek should be handled by upstream, we might need to re-download fragments */
        GST_DEBUG_OBJECT (qtdemux,
            "let upstream handle seek for fragmented playback");
        goto upstream;
      }

      /* Build complete index for seeking;
       * if not a fragmented file at least */
      if (!qtdemux->fragmented)
        if (!qtdemux_ensure_index (qtdemux))
          goto index_failed;
#ifndef GST_DISABLE_GST_DEBUG
      ts = gst_util_get_timestamp () - ts;
      GST_INFO_OBJECT (qtdemux,
          "Time taken to parse index %" GST_TIME_FORMAT, GST_TIME_ARGS (ts));
#endif
    }
      if (qtdemux->pullbased) {
        res = gst_qtdemux_do_seek (qtdemux, pad, event);
      } else if (gst_pad_push_event (qtdemux->sinkpad, gst_event_ref (event))) {
        GST_DEBUG_OBJECT (qtdemux, "Upstream successfully seeked");
        res = TRUE;
      } else if (qtdemux->state == QTDEMUX_STATE_MOVIE && qtdemux->n_streams
          && !qtdemux->fragmented) {
        res = gst_qtdemux_do_push_seek (qtdemux, pad, event);
      } else {
        GST_DEBUG_OBJECT (qtdemux,
            "ignoring seek in push mode in current state");
        res = FALSE;
      }
      gst_event_unref (event);
      break;
    default:
    upstream:
      res = gst_pad_event_default (pad, parent, event);
      break;
  }

done:
  return res;

  /* ERRORS */
index_failed:
  {
    GST_ERROR_OBJECT (qtdemux, "Index failed");
    gst_event_unref (event);
    res = FALSE;
    goto done;
  }
}