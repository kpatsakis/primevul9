g_file_real_delete_async (GFile               *file,
                          int                  io_priority,
                          GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             user_data)
{
  GTask *task;

  task = g_task_new (file, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_file_real_delete_async);
  g_task_set_priority (task, io_priority);
  g_task_run_in_thread (task, delete_async_thread);
  g_object_unref (task);
}