qtdemux_parse_protection_scheme_info (GstQTDemux * qtdemux,
    QtDemuxStream * stream, GNode * container, guint32 * original_fmt)
{
  GNode *sinf;
  GNode *frma;
  GNode *schm;
  GNode *schi;

  g_return_val_if_fail (qtdemux != NULL, FALSE);
  g_return_val_if_fail (stream != NULL, FALSE);
  g_return_val_if_fail (container != NULL, FALSE);
  g_return_val_if_fail (original_fmt != NULL, FALSE);

  sinf = qtdemux_tree_get_child_by_type (container, FOURCC_sinf);
  if (G_UNLIKELY (!sinf)) {
    if (stream->protection_scheme_type == FOURCC_cenc) {
      GST_ERROR_OBJECT (qtdemux, "sinf box does not contain schi box, which is "
          "mandatory for Common Encryption");
      return FALSE;
    }
    return TRUE;
  }

  frma = qtdemux_tree_get_child_by_type (sinf, FOURCC_frma);
  if (G_UNLIKELY (!frma)) {
    GST_ERROR_OBJECT (qtdemux, "sinf box does not contain mandatory frma box");
    return FALSE;
  }

  *original_fmt = QT_FOURCC ((const guint8 *) frma->data + 8);
  GST_DEBUG_OBJECT (qtdemux, "original stream format: '%" GST_FOURCC_FORMAT "'",
      GST_FOURCC_ARGS (*original_fmt));

  schm = qtdemux_tree_get_child_by_type (sinf, FOURCC_schm);
  if (!schm) {
    GST_DEBUG_OBJECT (qtdemux, "sinf box does not contain schm box");
    return FALSE;
  }
  stream->protection_scheme_type = QT_FOURCC ((const guint8 *) schm->data + 12);
  stream->protection_scheme_version =
      QT_UINT32 ((const guint8 *) schm->data + 16);

  GST_DEBUG_OBJECT (qtdemux,
      "protection_scheme_type: %" GST_FOURCC_FORMAT ", "
      "protection_scheme_version: %#010x",
      GST_FOURCC_ARGS (stream->protection_scheme_type),
      stream->protection_scheme_version);

  schi = qtdemux_tree_get_child_by_type (sinf, FOURCC_schi);
  if (!schi) {
    GST_DEBUG_OBJECT (qtdemux, "sinf box does not contain schi box");
    return FALSE;
  }
  if (stream->protection_scheme_type == FOURCC_cenc) {
    QtDemuxCencSampleSetInfo *info;
    GNode *tenc;
    const guint8 *tenc_data;
    guint32 isEncrypted;
    guint8 iv_size;
    const guint8 *default_kid;
    GstBuffer *kid_buf;

    if (G_UNLIKELY (!stream->protection_scheme_info))
      stream->protection_scheme_info =
          g_malloc0 (sizeof (QtDemuxCencSampleSetInfo));

    info = (QtDemuxCencSampleSetInfo *) stream->protection_scheme_info;

    tenc = qtdemux_tree_get_child_by_type (schi, FOURCC_tenc);
    if (!tenc) {
      GST_ERROR_OBJECT (qtdemux, "schi box does not contain tenc box, "
          "which is mandatory for Common Encryption");
      return FALSE;
    }
    tenc_data = (const guint8 *) tenc->data + 12;
    isEncrypted = QT_UINT24 (tenc_data);
    iv_size = QT_UINT8 (tenc_data + 3);
    default_kid = (tenc_data + 4);
    kid_buf = gst_buffer_new_allocate (NULL, 16, NULL);
    gst_buffer_fill (kid_buf, 0, default_kid, 16);
    if (info->default_properties)
      gst_structure_free (info->default_properties);
    info->default_properties =
        gst_structure_new ("application/x-cenc",
        "iv_size", G_TYPE_UINT, iv_size,
        "encrypted", G_TYPE_BOOLEAN, (isEncrypted == 1),
        "kid", GST_TYPE_BUFFER, kid_buf, NULL);
    GST_DEBUG_OBJECT (qtdemux, "default sample properties: "
        "is_encrypted=%u, iv_size=%u", isEncrypted, iv_size);
    gst_buffer_unref (kid_buf);
  }
  return TRUE;
}