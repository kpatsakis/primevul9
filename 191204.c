qtdemux_seek_offset (GstQTDemux * demux, guint64 offset)
{
  GstEvent *event;
  gboolean res = 0;

  GST_DEBUG_OBJECT (demux, "Seeking to %" G_GUINT64_FORMAT, offset);

  event =
      gst_event_new_seek (1.0, GST_FORMAT_BYTES,
      GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE, GST_SEEK_TYPE_SET, offset,
      GST_SEEK_TYPE_NONE, -1);

  /* store seqnum to drop flush events, they don't need to reach downstream */
  demux->offset_seek_seqnum = gst_event_get_seqnum (event);
  res = gst_pad_push_event (demux->sinkpad, event);
  demux->offset_seek_seqnum = 0;

  return res;
}