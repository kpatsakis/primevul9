replace_async_thread (GTask         *task,
                      gpointer       source_object,
                      gpointer       task_data,
                      GCancellable  *cancellable)
{
  GFileOutputStream *stream;
  ReplaceAsyncData *data = task_data;
  GError *error = NULL;

  stream = g_file_replace (G_FILE (source_object),
                           data->etag,
                           data->make_backup,
                           data->flags,
                           cancellable,
                           &error);

  if (stream)
    g_task_return_pointer (task, stream, g_object_unref);
  else
    g_task_return_error (task, error);
}