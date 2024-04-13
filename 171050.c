g_file_resolve_relative_path (GFile      *file,
                              const char *relative_path)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (relative_path != NULL, NULL);

  iface = G_FILE_GET_IFACE (file);

  return (* iface->resolve_relative_path) (file, relative_path);
}