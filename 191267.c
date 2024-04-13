gst_qtdemux_perform_seek (GstQTDemux * qtdemux, GstSegment * segment,
    guint32 seqnum, GstSeekFlags flags)
{
  gint64 desired_offset;
  gint n;

  desired_offset = segment->position;

  GST_DEBUG_OBJECT (qtdemux, "seeking to %" GST_TIME_FORMAT,
      GST_TIME_ARGS (desired_offset));

  /* may not have enough fragmented info to do this adjustment,
   * and we can't scan (and probably should not) at this time with
   * possibly flushing upstream */
  if ((flags & GST_SEEK_FLAG_KEY_UNIT) && !qtdemux->fragmented) {
    gint64 min_offset;

    gst_qtdemux_adjust_seek (qtdemux, desired_offset, TRUE, &min_offset, NULL);
    GST_DEBUG_OBJECT (qtdemux, "keyframe seek, align to %"
        GST_TIME_FORMAT, GST_TIME_ARGS (min_offset));
    desired_offset = min_offset;
  }

  /* and set all streams to the final position */
  gst_flow_combiner_reset (qtdemux->flowcombiner);
  qtdemux->segment_seqnum = seqnum;
  for (n = 0; n < qtdemux->n_streams; n++) {
    QtDemuxStream *stream = qtdemux->streams[n];

    stream->time_position = desired_offset;
    stream->accumulated_base = 0;
    stream->sample_index = -1;
    stream->offset_in_sample = 0;
    stream->segment_index = -1;
    stream->sent_eos = FALSE;

    if (segment->flags & GST_SEEK_FLAG_FLUSH)
      gst_segment_init (&stream->segment, GST_FORMAT_TIME);
  }
  segment->position = desired_offset;
  segment->time = desired_offset;
  if (segment->rate >= 0) {
    segment->start = desired_offset;

    /* we stop at the end */
    if (segment->stop == -1)
      segment->stop = segment->duration;
  } else {
    segment->stop = desired_offset;
  }

  if (qtdemux->fragmented)
    qtdemux->fragmented_seek_pending = TRUE;

  return TRUE;
}