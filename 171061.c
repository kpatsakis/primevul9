g_file_mount_mountable_finish (GFile         *file,
                               GAsyncResult  *result,
                               GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (result), NULL);

  if (g_async_result_legacy_propagate_error (result, error))
    return NULL;
  else if (g_async_result_is_tagged (result, g_file_mount_mountable))
    return g_task_propagate_pointer (G_TASK (result), error);

  iface = G_FILE_GET_IFACE (file);
  return (* iface->mount_mountable_finish) (file, result, error);
}