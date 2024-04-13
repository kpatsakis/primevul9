g_file_load_partial_contents_async (GFile                 *file,
                                    GCancellable          *cancellable,
                                    GFileReadMoreCallback  read_more_callback,
                                    GAsyncReadyCallback    callback,
                                    gpointer               user_data)
{
  LoadContentsData *data;

  g_return_if_fail (G_IS_FILE (file));

  data = g_new0 (LoadContentsData, 1);
  data->read_more_callback = read_more_callback;
  data->content = g_byte_array_new ();

  data->task = g_task_new (file, cancellable, callback, user_data);
  g_task_set_source_tag (data->task, g_file_load_partial_contents_async);
  g_task_set_task_data (data->task, data, (GDestroyNotify)load_contents_data_free);

  g_file_read_async (file,
                     0,
                     g_task_get_cancellable (data->task),
                     load_contents_open_callback,
                     data);
}