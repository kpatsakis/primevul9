qtdemux_parse_ftyp (GstQTDemux * qtdemux, const guint8 * buffer, gint length)
{
  /* counts as header data */
  qtdemux->header_size += length;

  /* only consider at least a sufficiently complete ftyp atom */
  if (length >= 20) {
    GstBuffer *buf;

    qtdemux->major_brand = QT_FOURCC (buffer + 8);
    GST_DEBUG_OBJECT (qtdemux, "major brand: %" GST_FOURCC_FORMAT,
        GST_FOURCC_ARGS (qtdemux->major_brand));
    if (qtdemux->comp_brands)
      gst_buffer_unref (qtdemux->comp_brands);
    buf = qtdemux->comp_brands = gst_buffer_new_and_alloc (length - 16);
    gst_buffer_fill (buf, 0, buffer + 16, length - 16);
  }
}