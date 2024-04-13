g_file_make_directory_finish (GFile         *file,
                              GAsyncResult  *result,
                              GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (result), FALSE);

  iface = G_FILE_GET_IFACE (file);
  return (* iface->make_directory_finish) (file, result, error);
}