g_file_load_bytes_finish (GFile         *file,
                          GAsyncResult  *result,
                          gchar        **etag_out,
                          GError       **error)
{
  GBytes *bytes;

  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (G_IS_TASK (result), NULL);
  g_return_val_if_fail (g_task_is_valid (G_TASK (result), file), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  bytes = g_task_propagate_pointer (G_TASK (result), error);

  if (etag_out != NULL)
    *etag_out = g_strdup (g_task_get_task_data (G_TASK (result)));

  return bytes;
}