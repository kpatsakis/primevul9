gst_matroska_demux_sync_streams (GstMatroskaDemux * demux)
{
  GstClockTime gap_threshold;
  gint stream_nr;

  GST_OBJECT_LOCK (demux);

  GST_LOG_OBJECT (demux, "Sync to %" GST_TIME_FORMAT,
      GST_TIME_ARGS (demux->common.segment.position));

  g_assert (demux->common.num_streams == demux->common.src->len);
  for (stream_nr = 0; stream_nr < demux->common.src->len; stream_nr++) {
    GstMatroskaTrackContext *context;

    context = g_ptr_array_index (demux->common.src, stream_nr);

    GST_LOG_OBJECT (demux,
        "Checking for resync on stream %d (%" GST_TIME_FORMAT ")", stream_nr,
        GST_TIME_ARGS (context->pos));

    /* Only send gap events on non-subtitle streams if lagging way behind.
     * The 0.5 second threshold for subtitle streams is also quite random. */
    if (context->type == GST_MATROSKA_TRACK_TYPE_SUBTITLE)
      gap_threshold = GST_SECOND / 2;
    else
      gap_threshold = 3 * GST_SECOND;

    /* Lag need only be considered if we have advanced into requested segment */
    if (GST_CLOCK_TIME_IS_VALID (context->pos) &&
        GST_CLOCK_TIME_IS_VALID (demux->common.segment.position) &&
        demux->common.segment.position > demux->common.segment.start &&
        context->pos + gap_threshold < demux->common.segment.position) {

      GstEvent *event;
      guint64 start = context->pos;
      guint64 stop = demux->common.segment.position - gap_threshold;

      GST_DEBUG_OBJECT (demux,
          "Synchronizing stream %d with other by advancing time from %"
          GST_TIME_FORMAT " to %" GST_TIME_FORMAT, stream_nr,
          GST_TIME_ARGS (start), GST_TIME_ARGS (stop));

      context->pos = stop;

      event = gst_event_new_gap (start, stop - start);
      GST_OBJECT_UNLOCK (demux);
      gst_pad_push_event (context->pad, event);
      GST_OBJECT_LOCK (demux);
    }
  }

  GST_OBJECT_UNLOCK (demux);
}