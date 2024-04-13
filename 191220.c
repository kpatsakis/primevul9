gst_qtdemux_find_segment (GstQTDemux * qtdemux, QtDemuxStream * stream,
    GstClockTime time_position)
{
  gint i;
  guint32 seg_idx;

  GST_LOG_OBJECT (stream->pad, "finding segment for %" GST_TIME_FORMAT,
      GST_TIME_ARGS (time_position));

  seg_idx = -1;
  for (i = 0; i < stream->n_segments; i++) {
    QtDemuxSegment *segment = &stream->segments[i];

    GST_LOG_OBJECT (stream->pad,
        "looking at segment %" GST_TIME_FORMAT "-%" GST_TIME_FORMAT,
        GST_TIME_ARGS (segment->time), GST_TIME_ARGS (segment->stop_time));

    /* For the last segment we include stop_time in the last segment */
    if (i < stream->n_segments - 1) {
      if (segment->time <= time_position && time_position < segment->stop_time) {
        GST_LOG_OBJECT (stream->pad, "segment %d matches", i);
        seg_idx = i;
        break;
      }
    } else {
      /* Last segment always matches */
      seg_idx = i;
      break;
    }
  }
  return seg_idx;
}