g_file_unmount_mountable_with_operation_finish (GFile         *file,
                                                GAsyncResult  *result,
                                                GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (result), FALSE);

  if (g_async_result_legacy_propagate_error (result, error))
    return FALSE;
  else if (g_async_result_is_tagged (result, g_file_unmount_mountable_with_operation))
    return g_task_propagate_boolean (G_TASK (result), error);

  iface = G_FILE_GET_IFACE (file);
  if (iface->unmount_mountable_with_operation_finish != NULL)
    return (* iface->unmount_mountable_with_operation_finish) (file, result, error);
  else
    return (* iface->unmount_mountable_finish) (file, result, error);
}