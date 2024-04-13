gst_matroska_demux_parse_contents (GstMatroskaDemux * demux, GstEbmlRead * ebml)
{
  GstFlowReturn ret = GST_FLOW_OK;
  guint32 id;

  DEBUG_ELEMENT_START (demux, ebml, "SeekHead");

  if ((ret = gst_ebml_read_master (ebml, &id)) != GST_FLOW_OK) {
    DEBUG_ELEMENT_STOP (demux, ebml, "SeekHead", ret);
    return ret;
  }

  while (ret == GST_FLOW_OK && gst_ebml_read_has_remaining (ebml, 1, TRUE)) {
    if ((ret = gst_ebml_peek_id (ebml, &id)) != GST_FLOW_OK)
      break;

    switch (id) {
      case GST_MATROSKA_ID_SEEKENTRY:
      {
        ret = gst_matroska_demux_parse_contents_seekentry (demux, ebml);
        /* Ignore EOS and errors here */
        if (ret != GST_FLOW_OK) {
          GST_DEBUG_OBJECT (demux, "Ignoring %s", gst_flow_get_name (ret));
          ret = GST_FLOW_OK;
        }
        break;
      }

      default:
        ret = gst_matroska_read_common_parse_skip (&demux->common,
            ebml, "SeekHead", id);
        break;
    }
  }

  DEBUG_ELEMENT_STOP (demux, ebml, "SeekHead", ret);

  /* Sort clusters by position for easier searching */
  if (demux->clusters)
    g_array_sort (demux->clusters, (GCompareFunc) gst_matroska_cluster_compare);

  return ret;
}