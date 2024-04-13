g_file_load_partial_contents_finish (GFile         *file,
                                     GAsyncResult  *res,
                                     char         **contents,
                                     gsize         *length,
                                     char         **etag_out,
                                     GError       **error)
{
  GTask *task;
  LoadContentsData *data;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (g_task_is_valid (res, file), FALSE);
  g_return_val_if_fail (contents != NULL, FALSE);

  task = G_TASK (res);

  if (!g_task_propagate_boolean (task, error))
    {
      if (length)
        *length = 0;
      return FALSE;
    }

  data = g_task_get_task_data (task);

  if (length)
    *length = data->pos;

  if (etag_out)
    {
      *etag_out = data->etag;
      data->etag = NULL;
    }

  /* Zero terminate */
  g_byte_array_set_size (data->content, data->pos + 1);
  data->content->data[data->pos] = 0;

  *contents = (char *)g_byte_array_free (data->content, FALSE);
  data->content = NULL;

  return TRUE;
}