qtdemux_find_stream (GstQTDemux * qtdemux, guint32 id)
{
  QtDemuxStream *stream;
  gint i;

  /* check */
  if (G_UNLIKELY (!id)) {
    GST_DEBUG_OBJECT (qtdemux, "invalid track id 0");
    return NULL;
  }

  /* try to get it fast and simple */
  if (G_LIKELY (id <= qtdemux->n_streams)) {
    stream = qtdemux->streams[id - 1];
    if (G_LIKELY (stream->track_id == id))
      return stream;
  }

  /* linear search otherwise */
  for (i = 0; i < qtdemux->n_streams; i++) {
    stream = qtdemux->streams[i];
    if (stream->track_id == id)
      return stream;
  }
  if (qtdemux->mss_mode) {
    /* mss should have only 1 stream anyway */
    return qtdemux->streams[0];
  }

  return NULL;
}