g_file_real_replace_async (GFile               *file,
                           const char          *etag,
                           gboolean             make_backup,
                           GFileCreateFlags     flags,
                           int                  io_priority,
                           GCancellable        *cancellable,
                           GAsyncReadyCallback  callback,
                           gpointer             user_data)
{
  GTask *task;
  ReplaceAsyncData *data;

  data = g_new0 (ReplaceAsyncData, 1);
  data->etag = g_strdup (etag);
  data->make_backup = make_backup;
  data->flags = flags;

  task = g_task_new (file, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_file_real_replace_async);
  g_task_set_task_data (task, data, (GDestroyNotify)replace_async_data_free);
  g_task_set_priority (task, io_priority);

  g_task_run_in_thread (task, replace_async_thread);
  g_object_unref (task);
}