g_file_delete_finish (GFile         *file,
                      GAsyncResult  *result,
                      GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (result), FALSE);

  if (g_async_result_legacy_propagate_error (result, error))
    return FALSE;

  iface = G_FILE_GET_IFACE (file);
  return (* iface->delete_file_finish) (file, result, error);
}