copy_async_thread (GTask        *task,
                   gpointer      source,
                   gpointer      task_data,
                   GCancellable *cancellable)
{
  CopyAsyncData *data = task_data;
  gboolean result;
  GError *error = NULL;

  result = g_file_copy (data->source,
                        data->destination,
                        data->flags,
                        cancellable,
                        (data->progress_cb != NULL) ? copy_async_progress_callback : NULL,
                        task,
                        &error);
  if (result)
    g_task_return_boolean (task, TRUE);
  else
    g_task_return_error (task, error);
}