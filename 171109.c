set_display_name_async_thread (GTask        *task,
                               gpointer      object,
                               gpointer      task_data,
                               GCancellable *cancellable)
{
  GError *error = NULL;
  char *name = task_data;
  GFile *file;

  file = g_file_set_display_name (G_FILE (object), name, cancellable, &error);

  if (file == NULL)
    g_task_return_error (task, error);
  else
    g_task_return_pointer (task, file, g_object_unref);
}