gst_matroska_demux_check_parse_error (GstMatroskaDemux * demux)
{
  if (!demux->streaming && demux->next_cluster_offset > 0) {
    /* just repositioning to where next cluster should be and try from there */
    GST_WARNING_OBJECT (demux, "parse error, trying next cluster expected at %"
        G_GUINT64_FORMAT, demux->next_cluster_offset);
    demux->common.offset = demux->next_cluster_offset;
    demux->next_cluster_offset = 0;
    return GST_FLOW_OK;
  } else {
    gint64 pos;
    GstFlowReturn ret;

    /* sigh, one last attempt above and beyond call of duty ...;
     * search for cluster mark following current pos */
    pos = demux->common.offset;
    GST_WARNING_OBJECT (demux, "parse error, looking for next cluster");
    if ((ret = gst_matroska_demux_search_cluster (demux, &pos, TRUE)) !=
        GST_FLOW_OK) {
      /* did not work, give up */
      return ret;
    } else {
      GST_DEBUG_OBJECT (demux, "... found at  %" G_GUINT64_FORMAT, pos);
      /* try that position */
      demux->common.offset = pos;
      return GST_FLOW_OK;
    }
  }
}