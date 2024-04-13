gst_matroska_demux_scan_back_for_keyframe_cluster (GstMatroskaDemux * demux,
    gint64 * cluster_offset, GstClockTime * cluster_time)
{
  GstClockTime stream_start_time = demux->stream_start_time;
  guint64 first_cluster_offset = demux->first_cluster_offset;
  gint64 off = *cluster_offset;
  ClusterInfo cluster = { 0, };

  GST_INFO_OBJECT (demux, "Checking if cluster starts with keyframe");
  while (off > first_cluster_offset) {
    if (!gst_matroska_demux_peek_cluster_info (demux, &cluster, off)) {
      GST_LOG_OBJECT (demux,
          "Couldn't get info on cluster @ %" G_GUINT64_FORMAT, off);
      break;
    }

    /* Keyframe? Then we're done */
    if (cluster.status == CLUSTER_STATUS_STARTS_WITH_KEYFRAME) {
      GST_LOG_OBJECT (demux,
          "Found keyframe at start of cluster @ %" G_GUINT64_FORMAT, off);
      break;
    }

    /* We only scan back if we *know* we landed on a cluster that
     * starts with a delta frame. */
    if (cluster.status != CLUSTER_STATUS_STARTS_WITH_DELTAUNIT) {
      GST_LOG_OBJECT (demux,
          "No delta frame at start of cluster @ %" G_GUINT64_FORMAT, off);
      break;
    }

    GST_DEBUG_OBJECT (demux, "Cluster starts with delta frame, backtracking");

    /* Don't scan back more than this much in time from the cluster we
     * originally landed on. This is mostly a sanity check in case a file
     * always has keyframes in the middle of clusters and never at the
     * beginning. Without this we would always scan back to the beginning
     * of the file in that case. */
    if (cluster.time != GST_CLOCK_TIME_NONE) {
      GstClockTimeDiff distance = GST_CLOCK_DIFF (cluster.time, *cluster_time);

      if (distance < 0 || distance > demux->max_backtrack_distance * GST_SECOND) {
        GST_DEBUG_OBJECT (demux, "Haven't found cluster with keyframe within "
            "%u secs of original seek target cluster, stopping",
            demux->max_backtrack_distance);
        break;
      }
    }

    /* If we have cluster prev_size we can skip back efficiently. If not,
     * we'll just do a brute force search for a cluster identifier */
    if (cluster.prev_size > 0 && off >= cluster.prev_size) {
      off -= cluster.prev_size;
    } else {
      GstFlowReturn flow;

      GST_LOG_OBJECT (demux, "Cluster has no or invalid prev size, searching "
          "for previous cluster instead then");

      flow = gst_matroska_demux_search_cluster (demux, &off, FALSE);
      if (flow != GST_FLOW_OK) {
        GST_DEBUG_OBJECT (demux, "cluster search yielded flow %s, stopping",
            gst_flow_get_name (flow));
        break;
      }
    }

    if (off <= first_cluster_offset) {
      GST_LOG_OBJECT (demux, "Reached first cluster, stopping");
      *cluster_offset = first_cluster_offset;
      *cluster_time = stream_start_time;
      return TRUE;
    }
    GST_LOG_OBJECT (demux, "Trying prev cluster @ %" G_GUINT64_FORMAT, off);
  }

  /* If we found a cluster starting with a keyframe jump to that instead,
   * otherwise leave everything as it was before */
  if (cluster.time != GST_CLOCK_TIME_NONE
      && (cluster.offset == first_cluster_offset
          || cluster.status == CLUSTER_STATUS_STARTS_WITH_KEYFRAME)) {
    *cluster_offset = cluster.offset;
    *cluster_time = cluster.time;
    return TRUE;
  }

  return FALSE;
}