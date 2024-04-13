qtdemux_parse_saiz (GstQTDemux * qtdemux, QtDemuxStream * stream,
    GstByteReader * br, guint32 * sample_count)
{
  guint32 flags = 0;
  guint8 *info_sizes;
  guint8 default_info_size;

  g_return_val_if_fail (qtdemux != NULL, NULL);
  g_return_val_if_fail (stream != NULL, NULL);
  g_return_val_if_fail (br != NULL, NULL);
  g_return_val_if_fail (sample_count != NULL, NULL);

  if (!gst_byte_reader_get_uint32_be (br, &flags))
    return NULL;

  if (flags & 0x1) {
    /* aux_info_type and aux_info_type_parameter are ignored */
    if (!gst_byte_reader_skip (br, 8))
      return NULL;
  }

  if (!gst_byte_reader_get_uint8 (br, &default_info_size))
    return NULL;
  GST_DEBUG_OBJECT (qtdemux, "default_info_size: %u", default_info_size);

  if (!gst_byte_reader_get_uint32_be (br, sample_count))
    return NULL;
  GST_DEBUG_OBJECT (qtdemux, "sample_count: %u", *sample_count);


  if (default_info_size == 0) {
    if (!gst_byte_reader_dup_data (br, *sample_count, &info_sizes)) {
      return NULL;
    }
  } else {
    info_sizes = g_new (guint8, *sample_count);
    memset (info_sizes, default_info_size, *sample_count);
  }

  return info_sizes;
}