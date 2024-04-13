g_file_get_path (GFile *file)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), NULL);

  iface = G_FILE_GET_IFACE (file);

  return (* iface->get_path) (file);
}