check_update_duration (GstQTDemux * qtdemux, GstClockTime duration)
{
  guint i;
  guint64 movdur;
  GstClockTime prevdur;

  movdur = GSTTIME_TO_QTTIME (qtdemux, duration);

  if (movdur > qtdemux->duration) {
    prevdur = QTTIME_TO_GSTTIME (qtdemux, qtdemux->duration);
    GST_DEBUG_OBJECT (qtdemux,
        "Updating total duration to %" GST_TIME_FORMAT " was %" GST_TIME_FORMAT,
        GST_TIME_ARGS (duration), GST_TIME_ARGS (prevdur));
    qtdemux->duration = movdur;
    GST_DEBUG_OBJECT (qtdemux,
        "qtdemux->segment.duration: %" GST_TIME_FORMAT " .stop: %"
        GST_TIME_FORMAT, GST_TIME_ARGS (qtdemux->segment.duration),
        GST_TIME_ARGS (qtdemux->segment.stop));
    if (qtdemux->segment.duration == prevdur) {
      /* If the current segment has duration/stop identical to previous duration
       * update them also (because they were set at that point in time with
       * the wrong duration */
      /* We convert the value *from* the timescale version to avoid rounding errors */
      GstClockTime fixeddur = QTTIME_TO_GSTTIME (qtdemux, movdur);
      GST_DEBUG_OBJECT (qtdemux, "Updated segment.duration and segment.stop");
      qtdemux->segment.duration = fixeddur;
      qtdemux->segment.stop = fixeddur;
    }
  }
  for (i = 0; i < qtdemux->n_streams; i++) {
    QtDemuxStream *stream = qtdemux->streams[i];
    if (stream) {
      movdur = GSTTIME_TO_QTSTREAMTIME (stream, duration);
      if (movdur > stream->duration) {
        GST_DEBUG_OBJECT (qtdemux,
            "Updating stream #%d duration to %" GST_TIME_FORMAT, i,
            GST_TIME_ARGS (duration));
        stream->duration = movdur;
        if (stream->dummy_segment) {
          /* Update all dummy values to new duration */
          stream->segments[0].stop_time = duration;
          stream->segments[0].duration = duration;
          stream->segments[0].media_stop = duration;

          /* let downstream know we possibly have a new stop time */
          if (stream->segment_index != -1) {
            GstClockTime pos;

            if (qtdemux->segment.rate >= 0) {
              pos = stream->segment.start;
            } else {
              pos = stream->segment.stop;
            }

            gst_qtdemux_stream_update_segment (qtdemux, stream,
                stream->segment_index, pos, NULL, NULL);
          }
        }
      }
    }
  }
}