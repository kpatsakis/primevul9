gst_qtdemux_stream_update_segment (GstQTDemux * qtdemux, QtDemuxStream * stream,
    gint seg_idx, GstClockTime offset, GstClockTime * _start,
    GstClockTime * _stop)
{
  QtDemuxSegment *segment;
  GstClockTime start = 0, stop = GST_CLOCK_TIME_NONE, time = 0;
  gdouble rate;
  GstEvent *event;

  /* update the current segment */
  stream->segment_index = seg_idx;

  /* get the segment */
  segment = &stream->segments[seg_idx];

  if (G_UNLIKELY (offset < segment->time)) {
    GST_WARNING_OBJECT (stream->pad, "offset < segment->time %" GST_TIME_FORMAT,
        GST_TIME_ARGS (segment->time));
    return FALSE;
  }

  /* segment lies beyond total indicated duration */
  if (G_UNLIKELY (qtdemux->segment.duration != GST_CLOCK_TIME_NONE &&
          segment->time > qtdemux->segment.duration)) {
    GST_WARNING_OBJECT (stream->pad, "file duration %" GST_TIME_FORMAT
        " < segment->time %" GST_TIME_FORMAT,
        GST_TIME_ARGS (qtdemux->segment.duration),
        GST_TIME_ARGS (segment->time));
    return FALSE;
  }

  gst_qtdemux_stream_segment_get_boundaries (qtdemux, stream, offset,
      &start, &stop, &time);

  GST_DEBUG_OBJECT (stream->pad, "new segment %d from %" GST_TIME_FORMAT
      " to %" GST_TIME_FORMAT ", time %" GST_TIME_FORMAT, seg_idx,
      GST_TIME_ARGS (start), GST_TIME_ARGS (stop), GST_TIME_ARGS (time));

  /* combine global rate with that of the segment */
  rate = segment->rate * qtdemux->segment.rate;

  /* Copy flags from main segment */
  stream->segment.flags = qtdemux->segment.flags;

  /* update the segment values used for clipping */
  stream->segment.offset = qtdemux->segment.offset;
  stream->segment.base = qtdemux->segment.base + stream->accumulated_base;
  stream->segment.applied_rate = qtdemux->segment.applied_rate;
  stream->segment.rate = rate;
  stream->segment.start = start + QTSTREAMTIME_TO_GSTTIME (stream,
      stream->cslg_shift);
  stream->segment.stop = stop + QTSTREAMTIME_TO_GSTTIME (stream,
      stream->cslg_shift);
  stream->segment.time = time;
  stream->segment.position = stream->segment.start;

  GST_DEBUG_OBJECT (stream->pad, "New segment: %" GST_SEGMENT_FORMAT,
      &stream->segment);

  /* now prepare and send the segment */
  if (stream->pad) {
    event = gst_event_new_segment (&stream->segment);
    if (qtdemux->segment_seqnum) {
      gst_event_set_seqnum (event, qtdemux->segment_seqnum);
    }
    gst_pad_push_event (stream->pad, event);
    /* assume we can send more data now */
    GST_PAD_LAST_FLOW_RETURN (stream->pad) = GST_FLOW_OK;
    /* clear to send tags on this pad now */
    gst_qtdemux_push_tags (qtdemux, stream);
  }

  if (_start)
    *_start = start;
  if (_stop)
    *_stop = stop;

  return TRUE;
}