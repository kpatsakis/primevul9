g_file_real_open_readwrite_finish (GFile         *file,
                                   GAsyncResult  *res,
                                   GError       **error)
{
  g_return_val_if_fail (g_task_is_valid (res, file), NULL);

  return g_task_propagate_pointer (G_TASK (res), error);
}