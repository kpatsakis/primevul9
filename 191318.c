qtdemux_find_atom (GstQTDemux * qtdemux, guint64 * offset,
    guint64 * length, guint32 fourcc)
{
  GstFlowReturn ret;
  guint32 lfourcc;
  GstBuffer *buf;

  GST_LOG_OBJECT (qtdemux, "finding fourcc %" GST_FOURCC_FORMAT " at offset %"
      G_GUINT64_FORMAT, GST_FOURCC_ARGS (fourcc), *offset);

  while (TRUE) {
    GstMapInfo map;

    buf = NULL;
    ret = gst_pad_pull_range (qtdemux->sinkpad, *offset, 16, &buf);
    if (G_UNLIKELY (ret != GST_FLOW_OK))
      goto locate_failed;
    if (G_UNLIKELY (gst_buffer_get_size (buf) != 16)) {
      /* likely EOF */
      ret = GST_FLOW_EOS;
      gst_buffer_unref (buf);
      goto locate_failed;
    }
    gst_buffer_map (buf, &map, GST_MAP_READ);
    extract_initial_length_and_fourcc (map.data, 16, length, &lfourcc);
    gst_buffer_unmap (buf, &map);
    gst_buffer_unref (buf);

    if (G_UNLIKELY (*length == 0)) {
      GST_DEBUG_OBJECT (qtdemux, "invalid length 0");
      ret = GST_FLOW_ERROR;
      goto locate_failed;
    }

    if (lfourcc == fourcc) {
      GST_DEBUG_OBJECT (qtdemux, "found fourcc at offset %" G_GUINT64_FORMAT,
          *offset);
      break;
    } else {
      GST_LOG_OBJECT (qtdemux,
          "skipping atom '%" GST_FOURCC_FORMAT "' at %" G_GUINT64_FORMAT,
          GST_FOURCC_ARGS (fourcc), *offset);
      *offset += *length;
    }
  }

  return GST_FLOW_OK;

locate_failed:
  {
    /* might simply have had last one */
    GST_DEBUG_OBJECT (qtdemux, "fourcc not found");
    return ret;
  }
}