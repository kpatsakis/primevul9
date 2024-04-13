systemd_unit_name_escape (const gchar *in)
{
  /* Adapted from systemd source */
  GString * const str = g_string_sized_new (strlen (in));

  for (; *in; in++)
    {
      if (g_ascii_isalnum (*in) || *in == ':' || *in == '_' || *in == '.')
        g_string_append_c (str, *in);
      else
        g_string_append_printf (str, "\\x%02x", *in);
    }
  return g_string_free (str, FALSE);
}