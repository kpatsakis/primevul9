gst_qtdemux_append_protection_system_id (GstQTDemux * qtdemux,
    const gchar * system_id)
{
  gint i;

  if (!qtdemux->protection_system_ids)
    qtdemux->protection_system_ids =
        g_ptr_array_new_with_free_func ((GDestroyNotify) g_free);
  /* Check whether we already have an entry for this system ID. */
  for (i = 0; i < qtdemux->protection_system_ids->len; ++i) {
    const gchar *id = g_ptr_array_index (qtdemux->protection_system_ids, i);
    if (g_ascii_strcasecmp (system_id, id) == 0) {
      return;
    }
  }
  GST_DEBUG_OBJECT (qtdemux, "Adding cenc protection system ID %s", system_id);
  g_ptr_array_add (qtdemux->protection_system_ids, g_ascii_strdown (system_id,
          -1));
}