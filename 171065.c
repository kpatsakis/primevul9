g_file_real_query_info_async (GFile               *file,
                              const char          *attributes,
                              GFileQueryInfoFlags  flags,
                              int                  io_priority,
                              GCancellable        *cancellable,
                              GAsyncReadyCallback  callback,
                              gpointer             user_data)
{
  GTask *task;
  QueryInfoAsyncData *data;

  data = g_new0 (QueryInfoAsyncData, 1);
  data->attributes = g_strdup (attributes);
  data->flags = flags;

  task = g_task_new (file, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_file_real_query_info_async);
  g_task_set_task_data (task, data, (GDestroyNotify)query_info_data_free);
  g_task_set_priority (task, io_priority);
  g_task_run_in_thread (task, query_info_async_thread);
  g_object_unref (task);
}