replace_contents_open_callback (GObject      *obj,
                                GAsyncResult *open_res,
                                gpointer      user_data)
{
  GFile *file = G_FILE (obj);
  GFileOutputStream *stream;
  ReplaceContentsData *data = user_data;
  GError *error = NULL;

  stream = g_file_replace_finish (file, open_res, &error);

  if (stream)
    {
      const gchar *content;
      gsize length;

      content = g_bytes_get_data (data->content, &length);
      g_output_stream_write_async (G_OUTPUT_STREAM (stream),
                                   content + data->pos,
                                   length - data->pos,
                                   0,
                                   g_task_get_cancellable (data->task),
                                   replace_contents_write_callback,
                                   data);
    }
  else
    {
      g_task_return_error (data->task, error);
      g_object_unref (data->task);
    }
}