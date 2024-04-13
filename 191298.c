gst_qtdemux_stream_segment_get_boundaries (GstQTDemux * qtdemux,
    QtDemuxStream * stream, GstClockTime offset,
    GstClockTime * _start, GstClockTime * _stop, GstClockTime * _time)
{
  GstClockTime seg_time;
  GstClockTime start, stop, time;
  QtDemuxSegment *segment;

  segment = &stream->segments[stream->segment_index];

  /* get time in this segment */
  seg_time = (offset - segment->time) * segment->rate;

  GST_LOG_OBJECT (stream->pad, "seg_time %" GST_TIME_FORMAT,
      GST_TIME_ARGS (seg_time));

  if (G_UNLIKELY (seg_time > segment->duration)) {
    GST_LOG_OBJECT (stream->pad,
        "seg_time > segment->duration %" GST_TIME_FORMAT,
        GST_TIME_ARGS (segment->duration));
    seg_time = segment->duration;
  }

  /* qtdemux->segment.stop is in outside-time-realm, whereas
   * segment->media_stop is in track-time-realm.
   *
   * In order to compare the two, we need to bring segment.stop
   * into the track-time-realm
   *
   * FIXME - does this comment still hold? Don't see any conversion here */

  stop = qtdemux->segment.stop;
  if (stop == GST_CLOCK_TIME_NONE)
    stop = qtdemux->segment.duration;
  if (stop == GST_CLOCK_TIME_NONE)
    stop = segment->media_stop;
  else
    stop =
        MIN (segment->media_stop, stop - segment->time + segment->media_start);

  if (G_UNLIKELY (QTSEGMENT_IS_EMPTY (segment))) {
    start = segment->time + seg_time;
    time = offset;
    stop = start - seg_time + segment->duration;
  } else if (qtdemux->segment.rate >= 0) {
    start = MIN (segment->media_start + seg_time, stop);
    time = offset;
  } else {
    if (segment->media_start >= qtdemux->segment.start) {
      time = segment->time;
    } else {
      time = segment->time + (qtdemux->segment.start - segment->media_start);
    }

    start = MAX (segment->media_start, qtdemux->segment.start);
    stop = MIN (segment->media_start + seg_time, stop);
  }

  *_start = start;
  *_stop = stop;
  *_time = time;
}