g_file_has_uri_scheme (GFile      *file,
                       const char *uri_scheme)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (uri_scheme != NULL, FALSE);

  iface = G_FILE_GET_IFACE (file);

  return (* iface->has_uri_scheme) (file, uri_scheme);
}