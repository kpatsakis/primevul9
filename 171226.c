g_file_real_query_filesystem_info_async (GFile               *file,
                                         const char          *attributes,
                                         int                  io_priority,
                                         GCancellable        *cancellable,
                                         GAsyncReadyCallback  callback,
                                         gpointer             user_data)
{
  GTask *task;

  task = g_task_new (file, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_file_real_query_filesystem_info_async);
  g_task_set_task_data (task, g_strdup (attributes), g_free);
  g_task_set_priority (task, io_priority);
  g_task_run_in_thread (task, query_filesystem_info_async_thread);
  g_object_unref (task);
}