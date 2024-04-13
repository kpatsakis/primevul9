g_file_real_trash_finish (GFile         *file,
                          GAsyncResult  *res,
                          GError       **error)
{
  g_return_val_if_fail (g_task_is_valid (res, file), FALSE);

  return g_task_propagate_boolean (G_TASK (res), error);
}