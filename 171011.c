g_file_query_default_handler_finish (GFile        *file,
                                     GAsyncResult *result,
                                     GError      **error)
{
  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (g_task_is_valid (result, file), NULL);

  return g_task_propagate_pointer (G_TASK (result), error);
}