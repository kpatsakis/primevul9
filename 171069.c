g_file_load_bytes_async (GFile               *file,
                         GCancellable        *cancellable,
                         GAsyncReadyCallback  callback,
                         gpointer             user_data)
{
  GError *error = NULL;
  GBytes *bytes;
  GTask *task;

  g_return_if_fail (G_IS_FILE (file));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (file, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_file_load_bytes_async);

  if (!g_file_has_uri_scheme (file, "resource"))
    {
      g_file_load_contents_async (file,
                                  cancellable,
                                  g_file_load_bytes_cb,
                                  g_steal_pointer (&task));
      return;
    }

  bytes = g_file_load_bytes (file, cancellable, NULL, &error);

  if (bytes == NULL)
    g_task_return_error (task, g_steal_pointer (&error));
  else
    g_task_return_pointer (task,
                           g_steal_pointer (&bytes),
                           (GDestroyNotify)g_bytes_unref);

  g_object_unref (task);
}