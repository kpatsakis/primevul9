g_file_is_native (GFile *file)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);

  iface = G_FILE_GET_IFACE (file);

  return (* iface->is_native) (file);
}