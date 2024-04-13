g_file_stop_mountable_finish (GFile         *file,
                              GAsyncResult  *result,
                              GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (result), FALSE);

  if (g_async_result_legacy_propagate_error (result, error))
    return FALSE;
  else if (g_async_result_is_tagged (result, g_file_stop_mountable))
    return g_task_propagate_boolean (G_TASK (result), error);

  iface = G_FILE_GET_IFACE (file);
  return (* iface->stop_mountable_finish) (file, result, error);
}