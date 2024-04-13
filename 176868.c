gst_matroska_demux_element_send_event (GstElement * element, GstEvent * event)
{
  GstMatroskaDemux *demux = GST_MATROSKA_DEMUX (element);
  gboolean res;

  g_return_val_if_fail (event != NULL, FALSE);

  if (GST_EVENT_TYPE (event) == GST_EVENT_SEEK) {
    /* no seeking until we are (safely) ready */
    if (demux->common.state != GST_MATROSKA_READ_STATE_DATA) {
      GST_DEBUG_OBJECT (demux,
          "not ready for seeking yet, deferring seek: %" GST_PTR_FORMAT, event);
      if (demux->deferred_seek_event)
        gst_event_unref (demux->deferred_seek_event);
      demux->deferred_seek_event = event;
      demux->deferred_seek_pad = NULL;
      return TRUE;
    }
    res = gst_matroska_demux_handle_seek_event (demux, NULL, event);
  } else {
    GST_WARNING_OBJECT (demux, "Unhandled event of type %s",
        GST_EVENT_TYPE_NAME (event));
    res = FALSE;
  }
  gst_event_unref (event);
  return res;
}