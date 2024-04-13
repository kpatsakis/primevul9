qtdemux_pull_mfro_mfra (GstQTDemux * qtdemux)
{
  GstMapInfo mfro_map = GST_MAP_INFO_INIT;
  GstMapInfo mfra_map = GST_MAP_INFO_INIT;
  GstBuffer *mfro = NULL, *mfra = NULL;
  GstFlowReturn flow;
  gboolean ret = FALSE;
  GNode *mfra_node, *tfra_node;
  guint64 mfra_offset = 0;
  guint32 fourcc, mfra_size;
  gint64 len;

  /* query upstream size in bytes */
  if (!gst_pad_peer_query_duration (qtdemux->sinkpad, GST_FORMAT_BYTES, &len))
    goto size_query_failed;

  /* mfro box should be at the very end of the file */
  flow = gst_qtdemux_pull_atom (qtdemux, len - 16, 16, &mfro);
  if (flow != GST_FLOW_OK)
    goto exit;

  gst_buffer_map (mfro, &mfro_map, GST_MAP_READ);

  fourcc = QT_FOURCC (mfro_map.data + 4);
  if (fourcc != FOURCC_mfro)
    goto exit;

  GST_INFO_OBJECT (qtdemux, "Found mfro box");
  if (mfro_map.size < 16)
    goto invalid_mfro_size;

  mfra_size = QT_UINT32 (mfro_map.data + 12);
  if (mfra_size >= len)
    goto invalid_mfra_size;

  mfra_offset = len - mfra_size;

  GST_INFO_OBJECT (qtdemux, "mfra offset: %" G_GUINT64_FORMAT ", size %u",
      mfra_offset, mfra_size);

  /* now get and parse mfra box */
  flow = gst_qtdemux_pull_atom (qtdemux, mfra_offset, mfra_size, &mfra);
  if (flow != GST_FLOW_OK)
    goto broken_file;

  gst_buffer_map (mfra, &mfra_map, GST_MAP_READ);

  mfra_node = g_node_new ((guint8 *) mfra_map.data);
  qtdemux_parse_node (qtdemux, mfra_node, mfra_map.data, mfra_map.size);

  tfra_node = qtdemux_tree_get_child_by_type (mfra_node, FOURCC_tfra);

  while (tfra_node) {
    qtdemux_parse_tfra (qtdemux, tfra_node);
    /* iterate all siblings */
    tfra_node = qtdemux_tree_get_sibling_by_type (tfra_node, FOURCC_tfra);
  }
  g_node_destroy (mfra_node);

  GST_INFO_OBJECT (qtdemux, "parsed movie fragment random access box (mfra)");
  ret = TRUE;

exit:

  if (mfro) {
    if (mfro_map.memory != NULL)
      gst_buffer_unmap (mfro, &mfro_map);
    gst_buffer_unref (mfro);
  }
  if (mfra) {
    if (mfra_map.memory != NULL)
      gst_buffer_unmap (mfra, &mfra_map);
    gst_buffer_unref (mfra);
  }
  return ret;

/* ERRORS */
size_query_failed:
  {
    GST_WARNING_OBJECT (qtdemux, "could not query upstream size");
    goto exit;
  }
invalid_mfro_size:
  {
    GST_WARNING_OBJECT (qtdemux, "mfro size is too small");
    goto exit;
  }
invalid_mfra_size:
  {
    GST_WARNING_OBJECT (qtdemux, "mfra_size in mfro box is invalid");
    goto exit;
  }
broken_file:
  {
    GST_WARNING_OBJECT (qtdemux, "bogus mfra offset or size, broken file");
    goto exit;
  }
}