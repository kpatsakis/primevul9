g_file_mount_enclosing_volume_finish (GFile         *location,
                                      GAsyncResult  *result,
                                      GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (location), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (result), FALSE);

  if (g_async_result_legacy_propagate_error (result, error))
    return FALSE;
  else if (g_async_result_is_tagged (result, g_file_mount_enclosing_volume))
    return g_task_propagate_boolean (G_TASK (result), error);

  iface = G_FILE_GET_IFACE (location);

  return (* iface->mount_enclosing_volume_finish) (location, result, error);
}