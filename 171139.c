load_contents_close_callback (GObject      *obj,
                              GAsyncResult *close_res,
                              gpointer      user_data)
{
  GInputStream *stream = G_INPUT_STREAM (obj);
  LoadContentsData *data = user_data;

  /* Ignore errors here, we're only reading anyway */
  g_input_stream_close_finish (stream, close_res, NULL);
  g_object_unref (stream);

  g_task_return_boolean (data->task, TRUE);
  g_object_unref (data->task);
}