gst_qtdemux_stream_send_initial_gap_segments (GstQTDemux * demux,
    QtDemuxStream * stream)
{
  gint i;

  /* Push any initial gap segments before proceeding to the
   * 'real' data */
  for (i = 0; i < stream->n_segments; i++) {
    gst_qtdemux_activate_segment (demux, stream, i, stream->time_position);

    if (QTSEGMENT_IS_EMPTY (&stream->segments[i])) {
      gst_qtdemux_send_gap_for_segment (demux, stream, i,
          stream->time_position);
    } else {
      /* Only support empty segment at the beginning followed by
       * one non-empty segment, this was checked when parsing the
       * edts atom, arriving here is unexpected */
      g_assert (i + 1 == stream->n_segments);
      break;
    }
  }
}