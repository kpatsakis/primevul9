qtdemux_parse_segments (GstQTDemux * qtdemux, QtDemuxStream * stream,
    GNode * trak)
{
  GNode *edts;
  /* accept edts if they contain gaps at start and there is only
   * one media segment */
  gboolean allow_pushbased_edts = TRUE;
  gint media_segments_count = 0;

  /* parse and prepare segment info from the edit list */
  GST_DEBUG_OBJECT (qtdemux, "looking for edit list container");
  stream->n_segments = 0;
  stream->segments = NULL;
  if ((edts = qtdemux_tree_get_child_by_type (trak, FOURCC_edts))) {
    GNode *elst;
    gint n_segments;
    gint i, count, entry_size;
    guint64 time;
    GstClockTime stime;
    guint8 *buffer;
    guint8 version;

    GST_DEBUG_OBJECT (qtdemux, "looking for edit list");
    if (!(elst = qtdemux_tree_get_child_by_type (edts, FOURCC_elst)))
      goto done;

    buffer = elst->data;

    version = QT_UINT8 (buffer + 8);
    entry_size = (version == 1) ? 20 : 12;

    n_segments = QT_UINT32 (buffer + 12);

    /* we might allocate a bit too much, at least allocate 1 segment */
    stream->segments = g_new (QtDemuxSegment, MAX (n_segments, 1));

    /* segments always start from 0 */
    time = 0;
    stime = 0;
    count = 0;
    for (i = 0; i < n_segments; i++) {
      guint64 duration;
      guint64 media_time;
      gboolean time_valid = TRUE;
      QtDemuxSegment *segment;
      guint32 rate_int;
      GstClockTime media_start = GST_CLOCK_TIME_NONE;

      if (version == 1) {
        media_time = QT_UINT64 (buffer + 24 + i * entry_size);
        duration = QT_UINT64 (buffer + 16 + i * entry_size);
        if (media_time == G_MAXUINT64)
          time_valid = FALSE;
      } else {
        media_time = QT_UINT32 (buffer + 20 + i * entry_size);
        duration = QT_UINT32 (buffer + 16 + i * entry_size);
        if (media_time == G_MAXUINT32)
          time_valid = FALSE;
      }

      if (time_valid)
        media_start = QTSTREAMTIME_TO_GSTTIME (stream, media_time);

      segment = &stream->segments[count++];

      /* time and duration expressed in global timescale */
      segment->time = stime;
      /* add non scaled values so we don't cause roundoff errors */
      if (duration || media_start == GST_CLOCK_TIME_NONE) {
        time += duration;
        stime = QTTIME_TO_GSTTIME (qtdemux, time);
        segment->duration = stime - segment->time;
      } else {
        /* zero duration does not imply media_start == media_stop
         * but, only specify media_start.*/
        stime = QTTIME_TO_GSTTIME (qtdemux, qtdemux->duration);
        if (GST_CLOCK_TIME_IS_VALID (stime) && time_valid
            && stime >= media_start) {
          segment->duration = stime - media_start;
        } else {
          segment->duration = GST_CLOCK_TIME_NONE;
        }
      }
      segment->stop_time = stime;

      segment->trak_media_start = media_time;
      /* media_time expressed in stream timescale */
      if (time_valid) {
        segment->media_start = media_start;
        segment->media_stop = segment->media_start + segment->duration;
        media_segments_count++;
      } else {
        segment->media_start = GST_CLOCK_TIME_NONE;
        segment->media_stop = GST_CLOCK_TIME_NONE;
      }
      rate_int =
          QT_UINT32 (buffer + ((version == 1) ? 32 : 24) + i * entry_size);

      if (rate_int <= 1) {
        /* 0 is not allowed, some programs write 1 instead of the floating point
         * value */
        GST_WARNING_OBJECT (qtdemux, "found suspicious rate %" G_GUINT32_FORMAT,
            rate_int);
        segment->rate = 1;
      } else {
        segment->rate = rate_int / 65536.0;
      }

      GST_DEBUG_OBJECT (qtdemux, "created segment %d time %" GST_TIME_FORMAT
          ", duration %" GST_TIME_FORMAT ", media_start %" GST_TIME_FORMAT
          " (%" G_GUINT64_FORMAT ") , media_stop %" GST_TIME_FORMAT
          " stop_time %" GST_TIME_FORMAT " rate %g, (%d) timescale %u",
          i, GST_TIME_ARGS (segment->time),
          GST_TIME_ARGS (segment->duration),
          GST_TIME_ARGS (segment->media_start), media_time,
          GST_TIME_ARGS (segment->media_stop),
          GST_TIME_ARGS (segment->stop_time), segment->rate, rate_int,
          stream->timescale);
      if (segment->stop_time > qtdemux->segment.stop) {
        GST_WARNING_OBJECT (qtdemux, "Segment %d "
            " extends to %" GST_TIME_FORMAT
            " past the end of the file duration %" GST_TIME_FORMAT
            " it will be truncated", i, GST_TIME_ARGS (segment->stop_time),
            GST_TIME_ARGS (qtdemux->segment.stop));
        qtdemux->segment.stop = segment->stop_time;
      }
    }
    GST_DEBUG_OBJECT (qtdemux, "found %d segments", count);
    stream->n_segments = count;
    if (media_segments_count != 1)
      allow_pushbased_edts = FALSE;
  }
done:

  /* push based does not handle segments, so act accordingly here,
   * and warn if applicable */
  if (!qtdemux->pullbased && !allow_pushbased_edts) {
    GST_WARNING_OBJECT (qtdemux, "streaming; discarding edit list segments");
    /* remove and use default one below, we stream like it anyway */
    g_free (stream->segments);
    stream->segments = NULL;
    stream->n_segments = 0;
  }

  /* no segments, create one to play the complete trak */
  if (stream->n_segments == 0) {
    GstClockTime stream_duration =
        QTSTREAMTIME_TO_GSTTIME (stream, stream->duration);

    if (stream->segments == NULL)
      stream->segments = g_new (QtDemuxSegment, 1);

    /* represent unknown our way */
    if (stream_duration == 0)
      stream_duration = GST_CLOCK_TIME_NONE;

    stream->segments[0].time = 0;
    stream->segments[0].stop_time = stream_duration;
    stream->segments[0].duration = stream_duration;
    stream->segments[0].media_start = 0;
    stream->segments[0].media_stop = stream_duration;
    stream->segments[0].rate = 1.0;
    stream->segments[0].trak_media_start = 0;

    GST_DEBUG_OBJECT (qtdemux, "created dummy segment %" GST_TIME_FORMAT,
        GST_TIME_ARGS (stream_duration));
    stream->n_segments = 1;
    stream->dummy_segment = TRUE;
  }
  GST_DEBUG_OBJECT (qtdemux, "using %d segments", stream->n_segments);

  return TRUE;
}