g_file_replace_contents_finish (GFile         *file,
                                GAsyncResult  *res,
                                char         **new_etag,
                                GError       **error)
{
  GTask *task;
  ReplaceContentsData *data;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (g_task_is_valid (res, file), FALSE);

  task = G_TASK (res);

  if (!g_task_propagate_boolean (task, error))
    return FALSE;

  data = g_task_get_task_data (task);

  if (new_etag)
    {
      *new_etag = data->etag;
      data->etag = NULL; /* Take ownership */
    }

  return TRUE;
}