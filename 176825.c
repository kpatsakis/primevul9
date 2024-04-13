gst_matroska_demux_align_buffer (GstMatroskaDemux * demux,
    GstBuffer * buffer, gsize alignment)
{
  GstMapInfo map;

  gst_buffer_map (buffer, &map, GST_MAP_READ);

  if (map.size < sizeof (guintptr)) {
    gst_buffer_unmap (buffer, &map);
    return buffer;
  }

  if (((guintptr) map.data) & (alignment - 1)) {
    GstBuffer *new_buffer;
    GstAllocationParams params = { 0, alignment - 1, 0, 0, };

    new_buffer = gst_buffer_new_allocate (NULL,
        gst_buffer_get_size (buffer), &params);

    /* Copy data "by hand", so ensure alignment is kept: */
    gst_buffer_fill (new_buffer, 0, map.data, map.size);

    gst_buffer_copy_into (new_buffer, buffer, GST_BUFFER_COPY_METADATA, 0, -1);
    GST_DEBUG_OBJECT (demux,
        "We want output aligned on %" G_GSIZE_FORMAT ", reallocated",
        alignment);

    gst_buffer_unmap (buffer, &map);
    gst_buffer_unref (buffer);

    return new_buffer;
  }

  gst_buffer_unmap (buffer, &map);
  return buffer;
}