gst_qtdemux_remove_stream (GstQTDemux * qtdemux, int i)
{
  g_assert (i >= 0 && i < qtdemux->n_streams && qtdemux->streams[i] != NULL);

  gst_qtdemux_stream_free (qtdemux, qtdemux->streams[i]);
  qtdemux->streams[i] = qtdemux->streams[qtdemux->n_streams - 1];
  qtdemux->streams[qtdemux->n_streams - 1] = NULL;
  qtdemux->n_streams--;
}