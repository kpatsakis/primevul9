gst_qtdemux_advance_sample (GstQTDemux * qtdemux, QtDemuxStream * stream)
{
  QtDemuxSample *sample;
  QtDemuxSegment *segment;

  /* get current segment */
  segment = &stream->segments[stream->segment_index];

  if (G_UNLIKELY (QTSEGMENT_IS_EMPTY (segment))) {
    GST_DEBUG_OBJECT (qtdemux, "Empty segment, no samples to advance");
    goto next_segment;
  }

  if (G_UNLIKELY (stream->sample_index >= stream->to_sample)) {
    /* Mark the stream as EOS */
    GST_DEBUG_OBJECT (qtdemux,
        "reached max allowed sample %u, mark EOS", stream->to_sample);
    stream->time_position = GST_CLOCK_TIME_NONE;
    return;
  }

  /* move to next sample */
  stream->sample_index++;
  stream->offset_in_sample = 0;

  /* reached the last sample, we need the next segment */
  if (G_UNLIKELY (stream->sample_index >= stream->n_samples))
    goto next_segment;

  if (!qtdemux_parse_samples (qtdemux, stream, stream->sample_index)) {
    GST_LOG_OBJECT (qtdemux, "Parsing of index %u failed!",
        stream->sample_index);
    return;
  }

  /* get next sample */
  sample = &stream->samples[stream->sample_index];

  /* see if we are past the segment */
  if (G_UNLIKELY (QTSAMPLE_DTS (stream, sample) >= segment->media_stop))
    goto next_segment;

  if (QTSAMPLE_DTS (stream, sample) >= segment->media_start) {
    /* inside the segment, update time_position, looks very familiar to
     * GStreamer segments, doesn't it? */
    stream->time_position =
        QTSAMPLE_DTS (stream, sample) - segment->media_start + segment->time;
  } else {
    /* not yet in segment, time does not yet increment. This means
     * that we are still prerolling keyframes to the decoder so it can
     * decode the first sample of the segment. */
    stream->time_position = segment->time;
  }
  return;

  /* move to the next segment */
next_segment:
  {
    GST_DEBUG_OBJECT (qtdemux, "segment %d ended ", stream->segment_index);

    if (stream->segment_index == stream->n_segments - 1) {
      /* are we at the end of the last segment, we're EOS */
      stream->time_position = GST_CLOCK_TIME_NONE;
    } else {
      /* else we're only at the end of the current segment */
      stream->time_position = segment->stop_time;
    }
    /* make sure we select a new segment */

    /* accumulate previous segments */
    if (GST_CLOCK_TIME_IS_VALID (stream->segment.stop))
      stream->accumulated_base +=
          (stream->segment.stop -
          stream->segment.start) / ABS (stream->segment.rate);

    stream->segment_index = -1;
  }
}