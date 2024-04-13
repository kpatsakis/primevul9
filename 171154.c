g_file_hash (gconstpointer file)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), 0);

  iface = G_FILE_GET_IFACE (file);

  return (* iface->hash) ((GFile *)file);
}