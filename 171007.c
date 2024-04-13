enumerate_children_async_thread (GTask         *task,
                                 gpointer       object,
                                 gpointer       task_data,
                                 GCancellable  *cancellable)
{
  QueryInfoAsyncData *data = task_data;
  GFileEnumerator *enumerator;
  GError *error = NULL;

  enumerator = g_file_enumerate_children (G_FILE (object), data->attributes, data->flags, cancellable, &error);
  if (error)
    g_task_return_error (task, error);
  else
    g_task_return_pointer (task, enumerator, g_object_unref);
}