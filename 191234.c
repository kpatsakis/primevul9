gst_qtdemux_configure_protected_caps (GstQTDemux * qtdemux,
    QtDemuxStream * stream)
{
  GstStructure *s;
  const gchar *selected_system;

  g_return_val_if_fail (qtdemux != NULL, FALSE);
  g_return_val_if_fail (stream != NULL, FALSE);
  g_return_val_if_fail (gst_caps_get_size (stream->caps) == 1, FALSE);

  if (stream->protection_scheme_type != FOURCC_cenc) {
    GST_ERROR_OBJECT (qtdemux, "unsupported protection scheme");
    return FALSE;
  }
  if (qtdemux->protection_system_ids == NULL) {
    GST_ERROR_OBJECT (qtdemux, "stream is protected using cenc, but no "
        "cenc protection system information has been found");
    return FALSE;
  }
  g_ptr_array_add (qtdemux->protection_system_ids, NULL);
  selected_system = gst_protection_select_system ((const gchar **)
      qtdemux->protection_system_ids->pdata);
  g_ptr_array_remove_index (qtdemux->protection_system_ids,
      qtdemux->protection_system_ids->len - 1);
  if (!selected_system) {
    GST_ERROR_OBJECT (qtdemux, "stream is protected, but no "
        "suitable decryptor element has been found");
    return FALSE;
  }

  s = gst_caps_get_structure (stream->caps, 0);
  if (!gst_structure_has_name (s, "application/x-cenc")) {
    gst_structure_set (s,
        "original-media-type", G_TYPE_STRING, gst_structure_get_name (s),
        GST_PROTECTION_SYSTEM_ID_CAPS_FIELD, G_TYPE_STRING, selected_system,
        NULL);
    gst_structure_set_name (s, "application/x-cenc");
  }
  return TRUE;
}