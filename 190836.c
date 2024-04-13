add_conversion_fields (GstCaps * caps)
{
  guint i, n;

  n = gst_caps_get_size (caps);
  for (i = 0; i < n; i++) {
    GstStructure *s = gst_caps_get_structure (caps, i);

    if (gst_structure_has_field (s, "stream-format")) {
      const GValue *v = gst_structure_get_value (s, "stream-format");

      if (G_VALUE_HOLDS_STRING (v)) {
        const gchar *str = g_value_get_string (v);

        if (strcmp (str, "adts") == 0 || strcmp (str, "raw") == 0) {
          GValue va = G_VALUE_INIT;
          GValue vs = G_VALUE_INIT;

          g_value_init (&va, GST_TYPE_LIST);
          g_value_init (&vs, G_TYPE_STRING);
          g_value_set_string (&vs, "adts");
          gst_value_list_append_value (&va, &vs);
          g_value_set_string (&vs, "raw");
          gst_value_list_append_value (&va, &vs);
          gst_structure_set_value (s, "stream-format", &va);
          g_value_unset (&va);
          g_value_unset (&vs);
        }
      } else if (GST_VALUE_HOLDS_LIST (v)) {
        gboolean contains_raw = FALSE;
        gboolean contains_adts = FALSE;
        guint m = gst_value_list_get_size (v), j;

        for (j = 0; j < m; j++) {
          const GValue *ve = gst_value_list_get_value (v, j);
          const gchar *str;

          if (G_VALUE_HOLDS_STRING (ve) && (str = g_value_get_string (ve))) {
            if (strcmp (str, "adts") == 0)
              contains_adts = TRUE;
            else if (strcmp (str, "raw") == 0)
              contains_raw = TRUE;
          }
        }

        if (contains_adts || contains_raw) {
          GValue va = G_VALUE_INIT;
          GValue vs = G_VALUE_INIT;

          g_value_init (&va, GST_TYPE_LIST);
          g_value_init (&vs, G_TYPE_STRING);
          g_value_copy (v, &va);

          if (!contains_raw) {
            g_value_set_string (&vs, "raw");
            gst_value_list_append_value (&va, &vs);
          }
          if (!contains_adts) {
            g_value_set_string (&vs, "adts");
            gst_value_list_append_value (&va, &vs);
          }

          gst_structure_set_value (s, "stream-format", &va);

          g_value_unset (&vs);
          g_value_unset (&va);
        }
      }
    }
  }
}