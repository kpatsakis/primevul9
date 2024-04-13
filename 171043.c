load_contents_fstat_callback (GObject      *obj,
                              GAsyncResult *stat_res,
                              gpointer      user_data)
{
  GInputStream *stream = G_INPUT_STREAM (obj);
  LoadContentsData *data = user_data;
  GFileInfo *info;

  info = g_file_input_stream_query_info_finish (G_FILE_INPUT_STREAM (stream),
                                                stat_res, NULL);
  if (info)
    {
      data->etag = g_strdup (g_file_info_get_etag (info));
      g_object_unref (info);
    }

  g_input_stream_close_async (stream, 0,
                              g_task_get_cancellable (data->task),
                              load_contents_close_callback, data);
}