replace_contents_write_callback (GObject      *obj,
                                 GAsyncResult *read_res,
                                 gpointer      user_data)
{
  GOutputStream *stream = G_OUTPUT_STREAM (obj);
  ReplaceContentsData *data = user_data;
  GError *error = NULL;
  gssize write_size;

  write_size = g_output_stream_write_finish (stream, read_res, &error);

  if (write_size <= 0)
    {
      /* Error or EOF, close the file */
      if (write_size < 0)
        {
          data->failed = TRUE;
          g_task_return_error (data->task, error);
        }
      g_output_stream_close_async (stream, 0,
                                   g_task_get_cancellable (data->task),
                                   replace_contents_close_callback, data);
    }
  else if (write_size > 0)
    {
      const gchar *content;
      gsize length;

      content = g_bytes_get_data (data->content, &length);
      data->pos += write_size;

      if (data->pos >= length)
        g_output_stream_close_async (stream, 0,
                                     g_task_get_cancellable (data->task),
                                     replace_contents_close_callback, data);
      else
        g_output_stream_write_async (stream,
                                     content + data->pos,
                                     length - data->pos,
                                     0,
                                     g_task_get_cancellable (data->task),
                                     replace_contents_write_callback,
                                     data);
    }
}