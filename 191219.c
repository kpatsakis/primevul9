gst_qtdemux_pull_atom (GstQTDemux * qtdemux, guint64 offset, guint64 size,
    GstBuffer ** buf)
{
  GstFlowReturn flow;
  GstMapInfo map;
  gsize bsize;

  if (G_UNLIKELY (size == 0)) {
    GstFlowReturn ret;
    GstBuffer *tmp = NULL;

    ret = gst_qtdemux_pull_atom (qtdemux, offset, sizeof (guint32), &tmp);
    if (ret != GST_FLOW_OK)
      return ret;

    gst_buffer_map (tmp, &map, GST_MAP_READ);
    size = QT_UINT32 (map.data);
    GST_DEBUG_OBJECT (qtdemux, "size 0x%08" G_GINT64_MODIFIER "x", size);

    gst_buffer_unmap (tmp, &map);
    gst_buffer_unref (tmp);
  }

  /* Sanity check: catch bogus sizes (fuzzed/broken files) */
  if (G_UNLIKELY (size > QTDEMUX_MAX_ATOM_SIZE)) {
    if (qtdemux->state != QTDEMUX_STATE_MOVIE && qtdemux->got_moov) {
      /* we're pulling header but already got most interesting bits,
       * so never mind the rest (e.g. tags) (that much) */
      GST_WARNING_OBJECT (qtdemux, "atom has bogus size %" G_GUINT64_FORMAT,
          size);
      return GST_FLOW_EOS;
    } else {
      GST_ELEMENT_ERROR (qtdemux, STREAM, DEMUX,
          (_("This file is invalid and cannot be played.")),
          ("atom has bogus size %" G_GUINT64_FORMAT, size));
      return GST_FLOW_ERROR;
    }
  }

  flow = gst_pad_pull_range (qtdemux->sinkpad, offset, size, buf);

  if (G_UNLIKELY (flow != GST_FLOW_OK))
    return flow;

  bsize = gst_buffer_get_size (*buf);
  /* Catch short reads - we don't want any partial atoms */
  if (G_UNLIKELY (bsize < size)) {
    GST_WARNING_OBJECT (qtdemux,
        "short read: %" G_GSIZE_FORMAT " < %" G_GUINT64_FORMAT, bsize, size);
    gst_buffer_unref (*buf);
    *buf = NULL;
    return GST_FLOW_EOS;
  }

  return flow;
}