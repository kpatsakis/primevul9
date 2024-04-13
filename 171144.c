g_file_load_bytes (GFile         *file,
                   GCancellable  *cancellable,
                   gchar        **etag_out,
                   GError       **error)
{
  gchar *contents;
  gsize len;

  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (etag_out != NULL)
    *etag_out = NULL;

  if (g_file_has_uri_scheme (file, "resource"))
    {
      GBytes *bytes;
      gchar *uri, *unescaped;

      uri = g_file_get_uri (file);
      unescaped = g_uri_unescape_string (uri + strlen ("resource://"), NULL);
      g_free (uri);

      bytes = g_resources_lookup_data (unescaped, G_RESOURCE_LOOKUP_FLAGS_NONE, error);
      g_free (unescaped);

      return bytes;
    }

  /* contents is guaranteed to be \0 terminated */
  if (g_file_load_contents (file, cancellable, &contents, &len, etag_out, error))
    return g_bytes_new_take (g_steal_pointer (&contents), len);

  return NULL;
}