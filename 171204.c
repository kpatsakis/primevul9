g_file_find_enclosing_mount_finish (GFile         *file,
                                    GAsyncResult  *res,
                                    GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), NULL);

  if (g_async_result_legacy_propagate_error (res, error))
    return NULL;

  iface = G_FILE_GET_IFACE (file);
  return (* iface->find_enclosing_mount_finish) (file, res, error);
}