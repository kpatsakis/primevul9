create_readwrite_async_thread (GTask        *task,
                               gpointer      object,
                               gpointer      task_data,
                               GCancellable *cancellable)
{
  GFileCreateFlags *data = task_data;
  GFileIOStream *stream;
  GError *error = NULL;

  stream = g_file_create_readwrite (G_FILE (object), *data, cancellable, &error);

  if (stream == NULL)
    g_task_return_error (task, error);
  else
    g_task_return_pointer (task, stream, g_object_unref);
}