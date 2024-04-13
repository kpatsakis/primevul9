query_filesystem_info_async_thread (GTask         *task,
                                    gpointer       object,
                                    gpointer       task_data,
                                    GCancellable  *cancellable)
{
  const char *attributes = task_data;
  GFileInfo *info;
  GError *error = NULL;

  info = g_file_query_filesystem_info (G_FILE (object), attributes, cancellable, &error);
  if (info)
    g_task_return_pointer (task, info, g_object_unref);
  else
    g_task_return_error (task, error);
}