replace_contents_close_callback (GObject      *obj,
                                 GAsyncResult *close_res,
                                 gpointer      user_data)
{
  GOutputStream *stream = G_OUTPUT_STREAM (obj);
  ReplaceContentsData *data = user_data;

  /* Ignore errors here, we're only reading anyway */
  g_output_stream_close_finish (stream, close_res, NULL);
  g_object_unref (stream);

  if (!data->failed)
    {
      data->etag = g_file_output_stream_get_etag (G_FILE_OUTPUT_STREAM (stream));
      g_task_return_boolean (data->task, TRUE);
    }
  g_object_unref (data->task);
}