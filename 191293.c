qtdemux_ensure_index (GstQTDemux * qtdemux)
{
  guint i;

  GST_DEBUG_OBJECT (qtdemux, "collecting all metadata for all streams");

  /* Build complete index */
  for (i = 0; i < qtdemux->n_streams; i++) {
    QtDemuxStream *stream = qtdemux->streams[i];

    if (!qtdemux_parse_samples (qtdemux, stream, stream->n_samples - 1))
      goto parse_error;
  }
  return TRUE;

  /* ERRORS */
parse_error:
  {
    GST_LOG_OBJECT (qtdemux,
        "Building complete index of stream %u for seeking failed!", i);
    return FALSE;
  }
}