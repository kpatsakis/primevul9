find_enclosing_mount_async_thread (GTask        *task,
                                   gpointer      object,
                                   gpointer      task_data,
                                   GCancellable *cancellable)
{
  GError *error = NULL;
  GMount *mount;

  mount = g_file_find_enclosing_mount (G_FILE (object), cancellable, &error);

  if (mount == NULL)
    g_task_return_error (task, error);
  else
    g_task_return_pointer (task, mount, g_object_unref);
}