g_file_trash_finish (GFile         *file,
                     GAsyncResult  *result,
                     GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (result), FALSE);

  iface = G_FILE_GET_IFACE (file);
  return (* iface->trash_finish) (file, result, error);
}