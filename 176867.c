gst_matroska_demux_check_read_size (GstMatroskaDemux * demux, guint64 bytes)
{
  if (G_UNLIKELY (bytes > MAX_BLOCK_SIZE)) {
    /* only a few blocks are expected/allowed to be large,
     * and will be recursed into, whereas others will be read and must fit */
    if (demux->streaming) {
      /* fatal in streaming case, as we can't step over easily */
      GST_ELEMENT_ERROR (demux, STREAM, DEMUX, (NULL),
          ("reading large block of size %" G_GUINT64_FORMAT " not supported; "
              "file might be corrupt.", bytes));
      return GST_FLOW_ERROR;
    } else {
      /* indicate higher level to quietly give up */
      GST_DEBUG_OBJECT (demux,
          "too large block of size %" G_GUINT64_FORMAT, bytes);
      return GST_FLOW_ERROR;
    }
  } else {
    return GST_FLOW_OK;
  }
}