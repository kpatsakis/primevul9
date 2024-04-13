gst_qtdemux_seek_to_previous_keyframe (GstQTDemux * qtdemux)
{
  guint8 n = 0;
  guint32 seg_idx = 0, k_index = 0;
  guint32 ref_seg_idx, ref_k_index;
  GstClockTime k_pos = 0, last_stop = 0;
  QtDemuxSegment *seg = NULL;
  QtDemuxStream *ref_str = NULL;
  guint64 seg_media_start_mov;  /* segment media start time in mov format */
  guint64 target_ts;

  /* Now we choose an arbitrary stream, get the previous keyframe timestamp
   * and finally align all the other streams on that timestamp with their
   * respective keyframes */
  for (n = 0; n < qtdemux->n_streams; n++) {
    QtDemuxStream *str = qtdemux->streams[n];

    /* No candidate yet, take the first stream */
    if (!ref_str) {
      ref_str = str;
      continue;
    }

    /* So that stream has a segment, we prefer video streams */
    if (str->subtype == FOURCC_vide) {
      ref_str = str;
      break;
    }
  }

  if (G_UNLIKELY (!ref_str)) {
    GST_DEBUG_OBJECT (qtdemux, "couldn't find any stream");
    goto eos;
  }

  if (G_UNLIKELY (!ref_str->from_sample)) {
    GST_DEBUG_OBJECT (qtdemux, "reached the beginning of the file");
    goto eos;
  }

  /* So that stream has been playing from from_sample to to_sample. We will
   * get the timestamp of the previous sample and search for a keyframe before
   * that. For audio streams we do an arbitrary jump in the past (10 samples) */
  if (ref_str->subtype == FOURCC_vide) {
    k_index = gst_qtdemux_find_keyframe (qtdemux, ref_str,
        ref_str->from_sample - 1);
  } else {
    if (ref_str->from_sample >= 10)
      k_index = ref_str->from_sample - 10;
    else
      k_index = 0;
  }

  target_ts =
      ref_str->samples[k_index].timestamp +
      ref_str->samples[k_index].pts_offset;

  /* get current segment for that stream */
  seg = &ref_str->segments[ref_str->segment_index];
  /* Use segment start in original timescale for comparisons */
  seg_media_start_mov = seg->trak_media_start;

  GST_LOG_OBJECT (qtdemux, "keyframe index %u ts %" G_GUINT64_FORMAT
      " seg start %" G_GUINT64_FORMAT " %" GST_TIME_FORMAT "\n",
      k_index, target_ts, seg_media_start_mov,
      GST_TIME_ARGS (seg->media_start));

  /* Crawl back through segments to find the one containing this I frame */
  while (target_ts < seg_media_start_mov) {
    GST_DEBUG_OBJECT (qtdemux,
        "keyframe position (sample %u) is out of segment %u " " target %"
        G_GUINT64_FORMAT " seg start %" G_GUINT64_FORMAT, k_index,
        ref_str->segment_index, target_ts, seg_media_start_mov);

    if (G_UNLIKELY (!ref_str->segment_index)) {
      /* Reached first segment, let's consider it's EOS */
      goto eos;
    }
    ref_str->segment_index--;
    seg = &ref_str->segments[ref_str->segment_index];
    /* Use segment start in original timescale for comparisons */
    seg_media_start_mov = seg->trak_media_start;
  }
  /* Calculate time position of the keyframe and where we should stop */
  k_pos =
      QTSTREAMTIME_TO_GSTTIME (ref_str,
      target_ts - seg->trak_media_start) + seg->time;
  last_stop =
      QTSTREAMTIME_TO_GSTTIME (ref_str,
      ref_str->samples[ref_str->from_sample].timestamp -
      seg->trak_media_start) + seg->time;

  GST_DEBUG_OBJECT (qtdemux, "preferred stream played from sample %u, "
      "now going to sample %u (pts %" GST_TIME_FORMAT ")", ref_str->from_sample,
      k_index, GST_TIME_ARGS (k_pos));

  /* Set last_stop with the keyframe timestamp we pushed of that stream */
  qtdemux->segment.position = last_stop;
  GST_DEBUG_OBJECT (qtdemux, "last_stop now is %" GST_TIME_FORMAT,
      GST_TIME_ARGS (last_stop));

  if (G_UNLIKELY (last_stop < qtdemux->segment.start)) {
    GST_DEBUG_OBJECT (qtdemux, "reached the beginning of segment");
    goto eos;
  }

  ref_seg_idx = ref_str->segment_index;
  ref_k_index = k_index;

  /* Align them all on this */
  for (n = 0; n < qtdemux->n_streams; n++) {
    guint32 index = 0;
    GstClockTime seg_time = 0;
    QtDemuxStream *str = qtdemux->streams[n];

    /* aligning reference stream again might lead to backing up to yet another
     * keyframe (due to timestamp rounding issues),
     * potentially putting more load on downstream; so let's try to avoid */
    if (str == ref_str) {
      seg_idx = ref_seg_idx;
      seg = &str->segments[seg_idx];
      k_index = ref_k_index;
      GST_DEBUG_OBJECT (qtdemux, "reference stream %d segment %d, "
          "sample at index %d", n, ref_str->segment_index, k_index);
    } else {
      seg_idx = gst_qtdemux_find_segment (qtdemux, str, k_pos);
      GST_DEBUG_OBJECT (qtdemux,
          "stream %d align segment %d for keyframe pos %" GST_TIME_FORMAT, n,
          seg_idx, GST_TIME_ARGS (k_pos));

      /* get segment and time in the segment */
      seg = &str->segments[seg_idx];
      seg_time = k_pos - seg->time;

      /* get the media time in the segment.
       * No adjustment for empty "filler" segments */
      if (seg->media_start != GST_CLOCK_TIME_NONE)
        seg_time += seg->media_start;

      /* get the index of the sample with media time */
      index = gst_qtdemux_find_index_linear (qtdemux, str, seg_time);
      GST_DEBUG_OBJECT (qtdemux,
          "stream %d sample for %" GST_TIME_FORMAT " at %u", n,
          GST_TIME_ARGS (seg_time), index);

      /* find previous keyframe */
      k_index = gst_qtdemux_find_keyframe (qtdemux, str, index);
    }

    /* Remember until where we want to go */
    str->to_sample = str->from_sample - 1;
    /* Define our time position */
    target_ts =
        str->samples[k_index].timestamp + str->samples[k_index].pts_offset;
    str->time_position = QTSTREAMTIME_TO_GSTTIME (str, target_ts) + seg->time;
    if (seg->media_start != GST_CLOCK_TIME_NONE)
      str->time_position -= seg->media_start;

    /* Now seek back in time */
    gst_qtdemux_move_stream (qtdemux, str, k_index);
    GST_DEBUG_OBJECT (qtdemux, "stream %d keyframe at %u, time position %"
        GST_TIME_FORMAT " playing from sample %u to %u", n, k_index,
        GST_TIME_ARGS (str->time_position), str->from_sample, str->to_sample);
  }

  return GST_FLOW_OK;

eos:
  return GST_FLOW_EOS;
}