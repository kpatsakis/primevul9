create_async_thread (GTask         *task,
                     gpointer       source_object,
                     gpointer       task_data,
                     GCancellable  *cancellable)
{
  GFileCreateFlags *data = task_data;
  GFileOutputStream *stream;
  GError *error = NULL;

  stream = g_file_create (G_FILE (source_object), *data, cancellable, &error);
  if (stream)
    g_task_return_pointer (task, stream, g_object_unref);
  else
    g_task_return_error (task, error);
}