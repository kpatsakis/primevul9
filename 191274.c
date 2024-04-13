qtdemux_is_brand_3gp (GstQTDemux * qtdemux, gboolean major)
{
  if (major) {
    return ((qtdemux->major_brand & GST_MAKE_FOURCC (255, 255, 0, 0)) ==
        FOURCC_3g__);
  } else if (qtdemux->comp_brands != NULL) {
    GstMapInfo map;
    guint8 *data;
    gsize size;
    gboolean res = FALSE;

    gst_buffer_map (qtdemux->comp_brands, &map, GST_MAP_READ);
    data = map.data;
    size = map.size;
    while (size >= 4) {
      res = res || ((QT_FOURCC (data) & GST_MAKE_FOURCC (255, 255, 0, 0)) ==
          FOURCC_3g__);
      data += 4;
      size -= 4;
    }
    gst_buffer_unmap (qtdemux->comp_brands, &map);
    return res;
  } else {
    return FALSE;
  }
}