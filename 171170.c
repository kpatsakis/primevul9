query_info_async_thread (GTask         *task,
                         gpointer       object,
                         gpointer       task_data,
                         GCancellable  *cancellable)
{
  QueryInfoAsyncData *data = task_data;
  GFileInfo *info;
  GError *error = NULL;

  info = g_file_query_info (G_FILE (object), data->attributes, data->flags, cancellable, &error);
  if (info)
    g_task_return_pointer (task, info, g_object_unref);
  else
    g_task_return_error (task, error);
}