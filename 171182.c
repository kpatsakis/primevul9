load_contents_open_callback (GObject      *obj,
                             GAsyncResult *open_res,
                             gpointer      user_data)
{
  GFile *file = G_FILE (obj);
  GFileInputStream *stream;
  LoadContentsData *data = user_data;
  GError *error = NULL;

  stream = g_file_read_finish (file, open_res, &error);

  if (stream)
    {
      g_byte_array_set_size (data->content,
                             data->pos + GET_CONTENT_BLOCK_SIZE);
      g_input_stream_read_async (G_INPUT_STREAM (stream),
                                 data->content->data + data->pos,
                                 GET_CONTENT_BLOCK_SIZE,
                                 0,
                                 g_task_get_cancellable (data->task),
                                 load_contents_read_callback,
                                 data);
    }
  else
    {
      g_task_return_error (data->task, error);
      g_object_unref (data->task);
    }
}