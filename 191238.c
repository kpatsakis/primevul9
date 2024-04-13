qtdemux_parse_cenc_aux_info (GstQTDemux * qtdemux, QtDemuxStream * stream,
    GstByteReader * br, guint8 * info_sizes, guint32 sample_count)
{
  QtDemuxCencSampleSetInfo *ss_info = NULL;
  guint8 size;
  gint i;

  g_return_val_if_fail (qtdemux != NULL, FALSE);
  g_return_val_if_fail (stream != NULL, FALSE);
  g_return_val_if_fail (br != NULL, FALSE);
  g_return_val_if_fail (stream->protected, FALSE);
  g_return_val_if_fail (stream->protection_scheme_info != NULL, FALSE);

  ss_info = (QtDemuxCencSampleSetInfo *) stream->protection_scheme_info;

  if (ss_info->crypto_info) {
    GST_LOG_OBJECT (qtdemux, "unreffing existing crypto_info");
    g_ptr_array_free (ss_info->crypto_info, TRUE);
  }

  ss_info->crypto_info =
      g_ptr_array_new_full (sample_count,
      (GDestroyNotify) qtdemux_gst_structure_free);

  for (i = 0; i < sample_count; ++i) {
    GstStructure *properties;
    guint16 n_subsamples = 0;
    guint8 *data;
    guint iv_size;
    GstBuffer *buf;

    properties = qtdemux_get_cenc_sample_properties (qtdemux, stream, i);
    if (properties == NULL) {
      GST_ERROR_OBJECT (qtdemux, "failed to get properties for sample %u", i);
      return FALSE;
    }
    if (!gst_structure_get_uint (properties, "iv_size", &iv_size)) {
      GST_ERROR_OBJECT (qtdemux, "failed to get iv_size for sample %u", i);
      gst_structure_free (properties);
      return FALSE;
    }
    if (!gst_byte_reader_dup_data (br, iv_size, &data)) {
      GST_ERROR_OBJECT (qtdemux, "failed to get IV for sample %u", i);
      gst_structure_free (properties);
      return FALSE;
    }
    buf = gst_buffer_new_wrapped (data, iv_size);
    gst_structure_set (properties, "iv", GST_TYPE_BUFFER, buf, NULL);
    gst_buffer_unref (buf);
    size = info_sizes[i];
    if (size > iv_size) {
      if (!gst_byte_reader_get_uint16_be (br, &n_subsamples)
          || !(n_subsamples > 0)) {
        gst_structure_free (properties);
        GST_ERROR_OBJECT (qtdemux,
            "failed to get subsample count for sample %u", i);
        return FALSE;
      }
      GST_LOG_OBJECT (qtdemux, "subsample count: %u", n_subsamples);
      if (!gst_byte_reader_dup_data (br, n_subsamples * 6, &data)) {
        GST_ERROR_OBJECT (qtdemux, "failed to get subsample data for sample %u",
            i);
        gst_structure_free (properties);
        return FALSE;
      }
      buf = gst_buffer_new_wrapped (data, n_subsamples * 6);
      if (!buf) {
        gst_structure_free (properties);
        return FALSE;
      }
      gst_structure_set (properties,
          "subsample_count", G_TYPE_UINT, n_subsamples,
          "subsamples", GST_TYPE_BUFFER, buf, NULL);
      gst_buffer_unref (buf);
    } else {
      gst_structure_set (properties, "subsample_count", G_TYPE_UINT, 0, NULL);
    }
    g_ptr_array_add (ss_info->crypto_info, properties);
  }
  return TRUE;
}