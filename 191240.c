gst_qtdemux_stream_flush_segments_data (GstQTDemux * qtdemux,
    QtDemuxStream * stream)
{
  g_free (stream->segments);
  stream->segments = NULL;
  stream->segment_index = -1;
  stream->accumulated_base = 0;
}