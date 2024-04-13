trash_async_thread (GTask        *task,
                    gpointer      object,
                    gpointer      task_data,
                    GCancellable *cancellable)
{
  GError *error = NULL;

  if (g_file_trash (G_FILE (object), cancellable, &error))
    g_task_return_boolean (task, TRUE);
  else
    g_task_return_error (task, error);
}