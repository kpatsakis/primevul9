gst_matroska_demux_send_event (GstMatroskaDemux * demux, GstEvent * event)
{
  gboolean ret = FALSE;
  gint i;

  g_return_val_if_fail (event != NULL, FALSE);

  GST_DEBUG_OBJECT (demux, "Sending event of type %s to all source pads",
      GST_EVENT_TYPE_NAME (event));

  g_assert (demux->common.src->len == demux->common.num_streams);
  for (i = 0; i < demux->common.src->len; i++) {
    GstMatroskaTrackContext *stream;

    stream = g_ptr_array_index (demux->common.src, i);
    gst_event_ref (event);
    gst_pad_push_event (stream->pad, event);
    ret = TRUE;
  }

  gst_event_unref (event);
  return ret;
}