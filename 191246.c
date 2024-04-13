qtdemux_add_fragmented_samples (GstQTDemux * qtdemux)
{
  guint64 length, offset;
  GstBuffer *buf = NULL;
  GstFlowReturn ret = GST_FLOW_OK;
  GstFlowReturn res = GST_FLOW_OK;
  GstMapInfo map;

  offset = qtdemux->moof_offset;
  GST_DEBUG_OBJECT (qtdemux, "next moof at offset %" G_GUINT64_FORMAT, offset);

  if (!offset) {
    GST_DEBUG_OBJECT (qtdemux, "no next moof");
    return GST_FLOW_EOS;
  }

  /* best not do pull etc with lock held */
  GST_OBJECT_UNLOCK (qtdemux);

  ret = qtdemux_find_atom (qtdemux, &offset, &length, FOURCC_moof);
  if (ret != GST_FLOW_OK)
    goto flow_failed;

  ret = gst_qtdemux_pull_atom (qtdemux, offset, length, &buf);
  if (G_UNLIKELY (ret != GST_FLOW_OK))
    goto flow_failed;
  gst_buffer_map (buf, &map, GST_MAP_READ);
  if (!qtdemux_parse_moof (qtdemux, map.data, map.size, offset, NULL)) {
    gst_buffer_unmap (buf, &map);
    gst_buffer_unref (buf);
    buf = NULL;
    goto parse_failed;
  }

  gst_buffer_unmap (buf, &map);
  gst_buffer_unref (buf);
  buf = NULL;

  offset += length;
  /* look for next moof */
  ret = qtdemux_find_atom (qtdemux, &offset, &length, FOURCC_moof);
  if (G_UNLIKELY (ret != GST_FLOW_OK))
    goto flow_failed;

exit:
  GST_OBJECT_LOCK (qtdemux);

  qtdemux->moof_offset = offset;

  return res;

parse_failed:
  {
    GST_DEBUG_OBJECT (qtdemux, "failed to parse moof");
    offset = 0;
    res = GST_FLOW_ERROR;
    goto exit;
  }
flow_failed:
  {
    /* maybe upstream temporarily flushing */
    if (ret != GST_FLOW_FLUSHING) {
      GST_DEBUG_OBJECT (qtdemux, "no next moof");
      offset = 0;
    } else {
      GST_DEBUG_OBJECT (qtdemux, "upstream WRONG_STATE");
      /* resume at current position next time */
    }
    res = ret;
    goto exit;
  }
}