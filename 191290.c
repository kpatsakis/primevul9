gst_qtdemux_activate_segment (GstQTDemux * qtdemux, QtDemuxStream * stream,
    guint32 seg_idx, GstClockTime offset)
{
  QtDemuxSegment *segment;
  guint32 index, kf_index;
  GstClockTime start = 0, stop = GST_CLOCK_TIME_NONE;

  GST_LOG_OBJECT (stream->pad, "activate segment %d, offset %" GST_TIME_FORMAT,
      seg_idx, GST_TIME_ARGS (offset));

  if (!gst_qtdemux_stream_update_segment (qtdemux, stream, seg_idx, offset,
          &start, &stop))
    return FALSE;

  segment = &stream->segments[stream->segment_index];

  /* in the fragmented case, we pick a fragment that starts before our
   * desired position and rely on downstream to wait for a keyframe
   * (FIXME: doesn't seem to work so well with ismv and wmv, as no parser; the
   * tfra entries tells us which trun/sample the key unit is in, but we don't
   * make use of this additional information at the moment) */
  if (qtdemux->fragmented) {
    stream->to_sample = G_MAXUINT32;
    return TRUE;
  }

  /* We don't need to look for a sample in push-based */
  if (!qtdemux->pullbased)
    return TRUE;

  /* and move to the keyframe before the indicated media time of the
   * segment */
  if (G_LIKELY (!QTSEGMENT_IS_EMPTY (segment))) {
    if (qtdemux->segment.rate >= 0) {
      index = gst_qtdemux_find_index_linear (qtdemux, stream, start);
      stream->to_sample = G_MAXUINT32;
      GST_DEBUG_OBJECT (stream->pad,
          "moving data pointer to %" GST_TIME_FORMAT ", index: %u, pts %"
          GST_TIME_FORMAT, GST_TIME_ARGS (start), index,
          GST_TIME_ARGS (QTSAMPLE_PTS (stream, &stream->samples[index])));
    } else {
      index = gst_qtdemux_find_index_linear (qtdemux, stream, stop);
      stream->to_sample = index;
      GST_DEBUG_OBJECT (stream->pad,
          "moving data pointer to %" GST_TIME_FORMAT ", index: %u, pts %"
          GST_TIME_FORMAT, GST_TIME_ARGS (stop), index,
          GST_TIME_ARGS (QTSAMPLE_PTS (stream, &stream->samples[index])));
    }
  } else {
    GST_DEBUG_OBJECT (stream->pad, "No need to look for keyframe, "
        "this is an empty segment");
    return TRUE;
  }

  /* gst_qtdemux_parse_sample () called from gst_qtdemux_find_index_linear ()
   * encountered an error and printed a message so we return appropriately */
  if (index == -1)
    return FALSE;

  /* we're at the right spot */
  if (index == stream->sample_index) {
    GST_DEBUG_OBJECT (stream->pad, "we are at the right index");
    return TRUE;
  }

  /* find keyframe of the target index */
  kf_index = gst_qtdemux_find_keyframe (qtdemux, stream, index);

/* *INDENT-OFF* */
/* indent does stupid stuff with stream->samples[].timestamp */

  /* if we move forwards, we don't have to go back to the previous
   * keyframe since we already sent that. We can also just jump to
   * the keyframe right before the target index if there is one. */
  if (index > stream->sample_index) {
    /* moving forwards check if we move past a keyframe */
    if (kf_index > stream->sample_index) {
      GST_DEBUG_OBJECT (stream->pad,
           "moving forwards to keyframe at %u (pts %" GST_TIME_FORMAT " dts %"GST_TIME_FORMAT" )", kf_index,
           GST_TIME_ARGS (QTSAMPLE_PTS(stream, &stream->samples[kf_index])),
           GST_TIME_ARGS (QTSAMPLE_DTS(stream, &stream->samples[kf_index])));
      gst_qtdemux_move_stream (qtdemux, stream, kf_index);
    } else {
      GST_DEBUG_OBJECT (stream->pad,
          "moving forwards, keyframe at %u (pts %" GST_TIME_FORMAT " dts %"GST_TIME_FORMAT" ) already sent", kf_index,
          GST_TIME_ARGS (QTSAMPLE_PTS (stream, &stream->samples[kf_index])),
          GST_TIME_ARGS (QTSAMPLE_DTS (stream, &stream->samples[kf_index])));
    }
  } else {
    GST_DEBUG_OBJECT (stream->pad,
        "moving backwards to keyframe at %u (pts %" GST_TIME_FORMAT " dts %"GST_TIME_FORMAT" )", kf_index,
        GST_TIME_ARGS (QTSAMPLE_PTS(stream, &stream->samples[kf_index])),
        GST_TIME_ARGS (QTSAMPLE_DTS(stream, &stream->samples[kf_index])));
    gst_qtdemux_move_stream (qtdemux, stream, kf_index);
  }

/* *INDENT-ON* */

  return TRUE;
}