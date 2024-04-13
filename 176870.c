gst_matroska_demux_add_prores_header (GstElement * element,
    GstMatroskaTrackContext * stream, GstBuffer ** buf)
{
  GstBuffer *newbuf = gst_buffer_new_allocate (NULL, 8, NULL);
  GstMapInfo map;
  guint32 frame_size;

  if (!gst_buffer_map (newbuf, &map, GST_MAP_WRITE)) {
    GST_ERROR ("Failed to map newly allocated buffer");
    return GST_FLOW_ERROR;
  }

  frame_size = gst_buffer_get_size (*buf);

  GST_WRITE_UINT32_BE (map.data, frame_size);
  map.data[4] = 'i';
  map.data[5] = 'c';
  map.data[6] = 'p';
  map.data[7] = 'f';

  gst_buffer_unmap (newbuf, &map);
  *buf = gst_buffer_append (newbuf, *buf);

  return GST_FLOW_OK;
}