perform_seek_to_offset (GstMatroskaDemux * demux, gdouble rate, guint64 offset,
    guint32 seqnum, GstSeekFlags flags)
{
  GstEvent *event;
  gboolean res = 0;

  GST_DEBUG_OBJECT (demux, "Seeking to %" G_GUINT64_FORMAT, offset);

  event =
      gst_event_new_seek (rate, GST_FORMAT_BYTES,
      flags | GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
      GST_SEEK_TYPE_SET, offset, GST_SEEK_TYPE_NONE, -1);
  gst_event_set_seqnum (event, seqnum);

  res = gst_pad_push_event (demux->common.sinkpad, event);

  /* segment event will update offset */
  return res;
}