replace_readwrite_async_thread (GTask        *task,
                                gpointer      object,
                                gpointer      task_data,
                                GCancellable *cancellable)
{
  GFileIOStream *stream;
  GError *error = NULL;
  ReplaceRWAsyncData *data = task_data;

  stream = g_file_replace_readwrite (G_FILE (object),
                                     data->etag,
                                     data->make_backup,
                                     data->flags,
                                     cancellable,
                                     &error);

  if (stream == NULL)
    g_task_return_error (task, error);
  else
    g_task_return_pointer (task, stream, g_object_unref);
}