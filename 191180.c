gst_qtdemux_sync_streams (GstQTDemux * demux)
{
  gint i;

  if (demux->n_streams <= 1)
    return;

  for (i = 0; i < demux->n_streams; i++) {
    QtDemuxStream *stream;
    GstClockTime end_time;

    stream = demux->streams[i];

    if (!stream->pad)
      continue;

    /* TODO advance time on subtitle streams here, if any some day */

    /* some clips/trailers may have unbalanced streams at the end,
     * so send EOS on shorter stream to prevent stalling others */

    /* do not mess with EOS if SEGMENT seeking */
    if (demux->segment.flags & GST_SEEK_FLAG_SEGMENT)
      continue;

    if (demux->pullbased) {
      /* loop mode is sample time based */
      if (!STREAM_IS_EOS (stream))
        continue;
    } else {
      /* push mode is byte position based */
      if (stream->n_samples &&
          stream->samples[stream->n_samples - 1].offset >= demux->offset)
        continue;
    }

    if (stream->sent_eos)
      continue;

    /* only act if some gap */
    end_time = stream->segments[stream->n_segments - 1].stop_time;
    GST_LOG_OBJECT (demux, "current position: %" GST_TIME_FORMAT
        ", stream end: %" GST_TIME_FORMAT,
        GST_TIME_ARGS (demux->segment.position), GST_TIME_ARGS (end_time));
    if (GST_CLOCK_TIME_IS_VALID (end_time)
        && (end_time + 2 * GST_SECOND < demux->segment.position)) {
      GstEvent *event;

      GST_DEBUG_OBJECT (demux, "sending EOS for stream %s",
          GST_PAD_NAME (stream->pad));
      stream->sent_eos = TRUE;
      event = gst_event_new_eos ();
      if (demux->segment_seqnum)
        gst_event_set_seqnum (event, demux->segment_seqnum);
      gst_pad_push_event (stream->pad, event);
    }
  }
}