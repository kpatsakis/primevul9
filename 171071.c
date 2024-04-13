g_file_real_set_attributes_async (GFile               *file,
                                  GFileInfo           *info,
                                  GFileQueryInfoFlags  flags,
                                  int                  io_priority,
                                  GCancellable        *cancellable,
                                  GAsyncReadyCallback  callback,
                                  gpointer             user_data)
{
  GTask *task;
  SetInfoAsyncData *data;

  data = g_new0 (SetInfoAsyncData, 1);
  data->info = g_file_info_dup (info);
  data->flags = flags;

  task = g_task_new (file, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_file_real_set_attributes_async);
  g_task_set_task_data (task, data, (GDestroyNotify)set_info_data_free);
  g_task_set_priority (task, io_priority);

  g_task_run_in_thread (task, set_info_async_thread);
  g_object_unref (task);
}