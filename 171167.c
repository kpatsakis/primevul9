open_read_async_thread (GTask         *task,
                        gpointer       object,
                        gpointer       task_data,
                        GCancellable  *cancellable)
{
  GFileInputStream *stream;
  GError *error = NULL;

  stream = g_file_read (G_FILE (object), cancellable, &error);
  if (stream)
    g_task_return_pointer (task, stream, g_object_unref);
  else
    g_task_return_error (task, error);
}