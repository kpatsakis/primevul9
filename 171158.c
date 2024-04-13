g_file_create_finish (GFile         *file,
                      GAsyncResult  *res,
                      GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), NULL);

  if (g_async_result_legacy_propagate_error (res, error))
    return NULL;

  iface = G_FILE_GET_IFACE (file);
  return (* iface->create_finish) (file, res, error);
}