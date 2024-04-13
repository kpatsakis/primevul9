load_contents_read_callback (GObject      *obj,
                             GAsyncResult *read_res,
                             gpointer      user_data)
{
  GInputStream *stream = G_INPUT_STREAM (obj);
  LoadContentsData *data = user_data;
  GError *error = NULL;
  gssize read_size;

  read_size = g_input_stream_read_finish (stream, read_res, &error);

  if (read_size < 0)
    {
      g_task_return_error (data->task, error);
      g_object_unref (data->task);

      /* Close the file ignoring any error */
      g_input_stream_close_async (stream, 0, NULL, NULL, NULL);
      g_object_unref (stream);
    }
  else if (read_size == 0)
    {
      g_file_input_stream_query_info_async (G_FILE_INPUT_STREAM (stream),
                                            G_FILE_ATTRIBUTE_ETAG_VALUE,
                                            0,
                                            g_task_get_cancellable (data->task),
                                            load_contents_fstat_callback,
                                            data);
    }
  else if (read_size > 0)
    {
      data->pos += read_size;

      g_byte_array_set_size (data->content,
                             data->pos + GET_CONTENT_BLOCK_SIZE);


      if (data->read_more_callback &&
          !data->read_more_callback ((char *)data->content->data, data->pos,
                                     g_async_result_get_user_data (G_ASYNC_RESULT (data->task))))
        g_file_input_stream_query_info_async (G_FILE_INPUT_STREAM (stream),
                                              G_FILE_ATTRIBUTE_ETAG_VALUE,
                                              0,
                                              g_task_get_cancellable (data->task),
                                              load_contents_fstat_callback,
                                              data);
      else
        g_input_stream_read_async (stream,
                                   data->content->data + data->pos,
                                   GET_CONTENT_BLOCK_SIZE,
                                   0,
                                   g_task_get_cancellable (data->task),
                                   load_contents_read_callback,
                                   data);
    }
}