g_file_real_append_to_async (GFile               *file,
                             GFileCreateFlags     flags,
                             int                  io_priority,
                             GCancellable        *cancellable,
                             GAsyncReadyCallback  callback,
                             gpointer             user_data)
{
  GFileCreateFlags *data;
  GTask *task;

  data = g_new0 (GFileCreateFlags, 1);
  *data = flags;

  task = g_task_new (file, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_file_real_append_to_async);
  g_task_set_task_data (task, data, g_free);
  g_task_set_priority (task, io_priority);

  g_task_run_in_thread (task, append_to_async_thread);
  g_object_unref (task);
}