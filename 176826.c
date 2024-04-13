gst_matroska_demux_take (GstMatroskaDemux * demux, guint64 bytes,
    GstEbmlRead * ebml)
{
  GstBuffer *buffer = NULL;
  GstFlowReturn ret = GST_FLOW_OK;

  GST_LOG_OBJECT (demux, "taking %" G_GUINT64_FORMAT " bytes for parsing",
      bytes);
  ret = gst_matroska_demux_check_read_size (demux, bytes);
  if (G_UNLIKELY (ret != GST_FLOW_OK)) {
    if (!demux->streaming) {
      /* in pull mode, we can skip */
      if ((ret = gst_matroska_demux_flush (demux, bytes)) == GST_FLOW_OK)
        ret = GST_FLOW_OVERFLOW;
    } else {
      /* otherwise fatal */
      ret = GST_FLOW_ERROR;
    }
    goto exit;
  }
  if (demux->streaming) {
    if (gst_adapter_available (demux->common.adapter) >= bytes)
      buffer = gst_adapter_take_buffer (demux->common.adapter, bytes);
    else
      ret = GST_FLOW_EOS;
  } else
    ret = gst_matroska_read_common_peek_bytes (&demux->common,
        demux->common.offset, bytes, &buffer, NULL);
  if (G_LIKELY (buffer)) {
    gst_ebml_read_init (ebml, GST_ELEMENT_CAST (demux), buffer,
        demux->common.offset);
    demux->common.offset += bytes;
  }
exit:
  return ret;
}