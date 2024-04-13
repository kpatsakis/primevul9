gst_qtdemux_prepare_current_sample (GstQTDemux * qtdemux,
    QtDemuxStream * stream, gboolean * empty, guint64 * offset, guint * size,
    GstClockTime * dts, GstClockTime * pts, GstClockTime * duration,
    gboolean * keyframe)
{
  QtDemuxSample *sample;
  GstClockTime time_position;
  guint32 seg_idx;

  g_return_val_if_fail (stream != NULL, FALSE);

  time_position = stream->time_position;
  if (G_UNLIKELY (time_position == GST_CLOCK_TIME_NONE))
    goto eos;

  seg_idx = stream->segment_index;
  if (G_UNLIKELY (seg_idx == -1)) {
    /* find segment corresponding to time_position if we are looking
     * for a segment. */
    seg_idx = gst_qtdemux_find_segment (qtdemux, stream, time_position);
  }

  /* different segment, activate it, sample_index will be set. */
  if (G_UNLIKELY (stream->segment_index != seg_idx))
    gst_qtdemux_activate_segment (qtdemux, stream, seg_idx, time_position);

  if (G_UNLIKELY (QTSEGMENT_IS_EMPTY (&stream->segments[stream->
                  segment_index]))) {
    QtDemuxSegment *seg = &stream->segments[stream->segment_index];

    GST_LOG_OBJECT (qtdemux, "Empty segment activated,"
        " prepare empty sample");

    *empty = TRUE;
    *pts = *dts = time_position;
    *duration = seg->duration - (time_position - seg->time);

    return TRUE;
  }

  *empty = FALSE;

  if (stream->sample_index == -1)
    stream->sample_index = 0;

  GST_LOG_OBJECT (qtdemux, "segment active, index = %u of %u",
      stream->sample_index, stream->n_samples);

  if (G_UNLIKELY (stream->sample_index >= stream->n_samples)) {
    if (!qtdemux->fragmented)
      goto eos;

    GST_INFO_OBJECT (qtdemux, "out of samples, trying to add more");
    do {
      GstFlowReturn flow;

      GST_OBJECT_LOCK (qtdemux);
      flow = qtdemux_add_fragmented_samples (qtdemux);
      GST_OBJECT_UNLOCK (qtdemux);

      if (flow != GST_FLOW_OK)
        goto eos;
    }
    while (stream->sample_index >= stream->n_samples);
  }

  if (!qtdemux_parse_samples (qtdemux, stream, stream->sample_index)) {
    GST_LOG_OBJECT (qtdemux, "Parsing of index %u failed!",
        stream->sample_index);
    return FALSE;
  }

  /* now get the info for the sample we're at */
  sample = &stream->samples[stream->sample_index];

  *dts = QTSAMPLE_DTS (stream, sample);
  *pts = QTSAMPLE_PTS (stream, sample);
  *offset = sample->offset;
  *size = sample->size;
  *duration = QTSAMPLE_DUR_DTS (stream, sample, *dts);
  *keyframe = QTSAMPLE_KEYFRAME (stream, sample);

  return TRUE;

  /* special cases */
eos:
  {
    stream->time_position = GST_CLOCK_TIME_NONE;
    return FALSE;
  }
}