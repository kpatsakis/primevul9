gst_matroska_demux_flush (GstMatroskaDemux * demux, guint flush)
{
  GST_LOG_OBJECT (demux, "skipping %d bytes", flush);
  demux->common.offset += flush;
  if (demux->streaming) {
    GstFlowReturn ret;

    /* hard to skip large blocks when streaming */
    ret = gst_matroska_demux_check_read_size (demux, flush);
    if (ret != GST_FLOW_OK)
      return ret;
    if (flush <= gst_adapter_available (demux->common.adapter))
      gst_adapter_flush (demux->common.adapter, flush);
    else
      return GST_FLOW_EOS;
  }
  return GST_FLOW_OK;
}