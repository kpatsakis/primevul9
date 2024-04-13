gst_qtdemux_adjust_seek (GstQTDemux * qtdemux, gint64 desired_time,
    gboolean use_sparse, gint64 * key_time, gint64 * key_offset)
{
  guint64 min_offset;
  gint64 min_byte_offset = -1;
  gint n;

  min_offset = desired_time;

  /* for each stream, find the index of the sample in the segment
   * and move back to the previous keyframe. */
  for (n = 0; n < qtdemux->n_streams; n++) {
    QtDemuxStream *str;
    guint32 index, kindex;
    guint32 seg_idx;
    GstClockTime media_start;
    GstClockTime media_time;
    GstClockTime seg_time;
    QtDemuxSegment *seg;
    gboolean empty_segment = FALSE;

    str = qtdemux->streams[n];

    if (str->sparse && !use_sparse)
      continue;

    seg_idx = gst_qtdemux_find_segment (qtdemux, str, desired_time);
    GST_DEBUG_OBJECT (qtdemux, "align segment %d", seg_idx);

    /* get segment and time in the segment */
    seg = &str->segments[seg_idx];
    seg_time = (desired_time - seg->time) * seg->rate;

    while (QTSEGMENT_IS_EMPTY (seg)) {
      seg_time = 0;
      empty_segment = TRUE;
      GST_DEBUG_OBJECT (str->pad, "Segment %d is empty, moving to next one",
          seg_idx);
      seg_idx++;
      if (seg_idx == str->n_segments)
        break;
      seg = &str->segments[seg_idx];
    }

    if (seg_idx == str->n_segments) {
      /* FIXME track shouldn't have the last segment as empty, but if it
       * happens we better handle it */
      continue;
    }

    /* get the media time in the segment */
    media_start = seg->media_start + seg_time;

    /* get the index of the sample with media time */
    index = gst_qtdemux_find_index_linear (qtdemux, str, media_start);
    GST_DEBUG_OBJECT (qtdemux, "sample for %" GST_TIME_FORMAT " at %u"
        " at offset %" G_GUINT64_FORMAT " (empty segment: %d)",
        GST_TIME_ARGS (media_start), index, str->samples[index].offset,
        empty_segment);

    if (!empty_segment) {
      /* find previous keyframe */
      kindex = gst_qtdemux_find_keyframe (qtdemux, str, index);

      /* if the keyframe is at a different position, we need to update the
       * requested seek time */
      if (index != kindex) {
        index = kindex;

        /* get timestamp of keyframe */
        media_time = QTSAMPLE_DTS (str, &str->samples[kindex]);
        GST_DEBUG_OBJECT (qtdemux,
            "keyframe at %u with time %" GST_TIME_FORMAT " at offset %"
            G_GUINT64_FORMAT, kindex, GST_TIME_ARGS (media_time),
            str->samples[kindex].offset);

        /* keyframes in the segment get a chance to change the
         * desired_offset. keyframes out of the segment are
         * ignored. */
        if (media_time >= seg->media_start) {
          GstClockTime seg_time;

          /* this keyframe is inside the segment, convert back to
           * segment time */
          seg_time = (media_time - seg->media_start) + seg->time;
          if (seg_time < min_offset)
            min_offset = seg_time;
        }
      }
    }

    if (min_byte_offset < 0 || str->samples[index].offset < min_byte_offset)
      min_byte_offset = str->samples[index].offset;
  }

  if (key_time)
    *key_time = min_offset;
  if (key_offset)
    *key_offset = min_byte_offset;
}