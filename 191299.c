gst_qtdemux_send_gap_for_segment (GstQTDemux * demux,
    QtDemuxStream * stream, gint segment_index, GstClockTime pos)
{
  GstClockTime ts, dur;
  GstEvent *gap;

  ts = pos;
  dur =
      stream->segments[segment_index].duration - (pos -
      stream->segments[segment_index].time);
  gap = gst_event_new_gap (ts, dur);
  stream->time_position += dur;

  GST_DEBUG_OBJECT (stream->pad, "Pushing gap for empty "
      "segment: %" GST_PTR_FORMAT, gap);
  gst_pad_push_event (stream->pad, gap);
}