gst_qtdemux_stream_free (GstQTDemux * qtdemux, QtDemuxStream * stream)
{
  gst_qtdemux_stream_clear (qtdemux, stream);
  if (stream->caps)
    gst_caps_unref (stream->caps);
  stream->caps = NULL;
  if (stream->pad) {
    gst_element_remove_pad (GST_ELEMENT_CAST (qtdemux), stream->pad);
    gst_flow_combiner_remove_pad (qtdemux->flowcombiner, stream->pad);
  }
  g_free (stream);
}