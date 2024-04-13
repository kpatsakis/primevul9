gst_qtdemux_find_sample (GstQTDemux * qtdemux, gint64 byte_pos, gboolean fw,
    gboolean set, QtDemuxStream ** _stream, gint * _index, gint64 * _time)
{
  gint i, n, index;
  gint64 time, min_time;
  QtDemuxStream *stream;

  min_time = -1;
  stream = NULL;
  index = -1;

  for (n = 0; n < qtdemux->n_streams; ++n) {
    QtDemuxStream *str;
    gint inc;
    gboolean set_sample;

    str = qtdemux->streams[n];
    set_sample = !set;

    if (fw) {
      i = 0;
      inc = 1;
    } else {
      i = str->n_samples - 1;
      inc = -1;
    }

    for (; (i >= 0) && (i < str->n_samples); i += inc) {
      if (str->samples[i].size == 0)
        continue;

      if (fw && (str->samples[i].offset < byte_pos))
        continue;

      if (!fw && (str->samples[i].offset + str->samples[i].size > byte_pos))
        continue;

      /* move stream to first available sample */
      if (set) {
        gst_qtdemux_move_stream (qtdemux, str, i);
        set_sample = TRUE;
      }

      /* avoid index from sparse streams since they might be far away */
      if (!str->sparse) {
        /* determine min/max time */
        time = QTSAMPLE_PTS (str, &str->samples[i]);
        if (min_time == -1 || (!fw && time > min_time) ||
            (fw && time < min_time)) {
          min_time = time;
        }

        /* determine stream with leading sample, to get its position */
        if (!stream ||
            (fw && (str->samples[i].offset < stream->samples[index].offset)) ||
            (!fw && (str->samples[i].offset > stream->samples[index].offset))) {
          stream = str;
          index = i;
        }
      }
      break;
    }

    /* no sample for this stream, mark eos */
    if (!set_sample)
      gst_qtdemux_move_stream (qtdemux, str, str->n_samples);
  }

  if (_time)
    *_time = min_time;
  if (_stream)
    *_stream = stream;
  if (_index)
    *_index = index;
}