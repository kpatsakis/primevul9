g_file_copy_finish (GFile         *file,
                    GAsyncResult  *res,
                    GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (res), FALSE);

  if (g_async_result_legacy_propagate_error (res, error))
    return FALSE;

  iface = G_FILE_GET_IFACE (file);
  return (* iface->copy_finish) (file, res, error);
}