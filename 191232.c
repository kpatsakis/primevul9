qtdemux_parse_piff (GstQTDemux * qtdemux, const guint8 * buffer, gint length,
    guint offset)
{
  GstByteReader br;
  guint8 version;
  guint32 flags = 0;
  guint i;
  guint8 iv_size = 8;
  QtDemuxStream *stream;
  GstStructure *structure;
  QtDemuxCencSampleSetInfo *ss_info = NULL;
  const gchar *system_id;
  gboolean uses_sub_sample_encryption = FALSE;

  if (qtdemux->n_streams == 0)
    return;

  stream = qtdemux->streams[0];

  structure = gst_caps_get_structure (stream->caps, 0);
  if (!gst_structure_has_name (structure, "application/x-cenc")) {
    GST_WARNING_OBJECT (qtdemux,
        "Attempting PIFF box parsing on an unencrypted stream.");
    return;
  }

  gst_structure_get (structure, GST_PROTECTION_SYSTEM_ID_CAPS_FIELD,
      G_TYPE_STRING, &system_id, NULL);
  gst_qtdemux_append_protection_system_id (qtdemux, system_id);

  stream->protected = TRUE;
  stream->protection_scheme_type = FOURCC_cenc;

  if (!stream->protection_scheme_info)
    stream->protection_scheme_info = g_new0 (QtDemuxCencSampleSetInfo, 1);

  ss_info = (QtDemuxCencSampleSetInfo *) stream->protection_scheme_info;

  if (ss_info->default_properties)
    gst_structure_free (ss_info->default_properties);

  ss_info->default_properties =
      gst_structure_new ("application/x-cenc",
      "iv_size", G_TYPE_UINT, iv_size, "encrypted", G_TYPE_BOOLEAN, TRUE, NULL);

  if (ss_info->crypto_info) {
    GST_LOG_OBJECT (qtdemux, "unreffing existing crypto_info");
    g_ptr_array_free (ss_info->crypto_info, TRUE);
    ss_info->crypto_info = NULL;
  }

  /* skip UUID */
  gst_byte_reader_init (&br, buffer + offset + 16, length - offset - 16);

  if (!gst_byte_reader_get_uint8 (&br, &version)) {
    GST_ERROR_OBJECT (qtdemux, "Error getting box's version field");
    return;
  }

  if (!gst_byte_reader_get_uint24_be (&br, &flags)) {
    GST_ERROR_OBJECT (qtdemux, "Error getting box's flags field");
    return;
  }

  if ((flags & 0x000001)) {
    guint32 algorithm_id = 0;
    const guint8 *kid;
    GstBuffer *kid_buf;
    gboolean is_encrypted = TRUE;

    if (!gst_byte_reader_get_uint24_le (&br, &algorithm_id)) {
      GST_ERROR_OBJECT (qtdemux, "Error getting box's algorithm ID field");
      return;
    }

    algorithm_id >>= 8;
    if (algorithm_id == 0) {
      is_encrypted = FALSE;
    } else if (algorithm_id == 1) {
      /* FIXME: maybe store this in properties? */
      GST_DEBUG_OBJECT (qtdemux, "AES 128-bits CTR encrypted stream");
    } else if (algorithm_id == 2) {
      /* FIXME: maybe store this in properties? */
      GST_DEBUG_OBJECT (qtdemux, "AES 128-bits CBC encrypted stream");
    }

    if (!gst_byte_reader_get_uint8 (&br, &iv_size))
      return;

    if (!gst_byte_reader_get_data (&br, 16, &kid))
      return;

    kid_buf = gst_buffer_new_allocate (NULL, 16, NULL);
    gst_buffer_fill (kid_buf, 0, kid, 16);
    if (ss_info->default_properties)
      gst_structure_free (ss_info->default_properties);
    ss_info->default_properties =
        gst_structure_new ("application/x-cenc",
        "iv_size", G_TYPE_UINT, iv_size,
        "encrypted", G_TYPE_BOOLEAN, is_encrypted,
        "kid", GST_TYPE_BUFFER, kid_buf, NULL);
    GST_DEBUG_OBJECT (qtdemux, "default sample properties: "
        "is_encrypted=%u, iv_size=%u", is_encrypted, iv_size);
    gst_buffer_unref (kid_buf);
  } else if ((flags & 0x000002)) {
    uses_sub_sample_encryption = TRUE;
  }

  if (!gst_byte_reader_get_uint32_be (&br, &qtdemux->cenc_aux_sample_count)) {
    GST_ERROR_OBJECT (qtdemux, "Error getting box's sample count field");
    return;
  }

  ss_info->crypto_info =
      g_ptr_array_new_full (qtdemux->cenc_aux_sample_count,
      (GDestroyNotify) qtdemux_gst_structure_free);

  for (i = 0; i < qtdemux->cenc_aux_sample_count; ++i) {
    GstStructure *properties;
    guint8 *data;
    GstBuffer *buf;

    properties = qtdemux_get_cenc_sample_properties (qtdemux, stream, i);
    if (properties == NULL) {
      GST_ERROR_OBJECT (qtdemux, "failed to get properties for sample %u", i);
      return;
    }

    if (!gst_byte_reader_dup_data (&br, iv_size, &data)) {
      GST_ERROR_OBJECT (qtdemux, "IV data not present for sample %u", i);
      gst_structure_free (properties);
      return;
    }
    buf = gst_buffer_new_wrapped (data, iv_size);
    gst_structure_set (properties, "iv", GST_TYPE_BUFFER, buf, NULL);
    gst_buffer_unref (buf);

    if (uses_sub_sample_encryption) {
      guint16 n_subsamples;

      if (!gst_byte_reader_get_uint16_be (&br, &n_subsamples)
          || n_subsamples == 0) {
        GST_ERROR_OBJECT (qtdemux,
            "failed to get subsample count for sample %u", i);
        gst_structure_free (properties);
        return;
      }
      GST_LOG_OBJECT (qtdemux, "subsample count: %u", n_subsamples);
      if (!gst_byte_reader_dup_data (&br, n_subsamples * 6, &data)) {
        GST_ERROR_OBJECT (qtdemux, "failed to get subsample data for sample %u",
            i);
        gst_structure_free (properties);
        return;
      }
      buf = gst_buffer_new_wrapped (data, n_subsamples * 6);
      gst_structure_set (properties,
          "subsample_count", G_TYPE_UINT, n_subsamples,
          "subsamples", GST_TYPE_BUFFER, buf, NULL);
      gst_buffer_unref (buf);
    } else {
      gst_structure_set (properties, "subsample_count", G_TYPE_UINT, 0, NULL);
    }

    g_ptr_array_add (ss_info->crypto_info, properties);
  }
}