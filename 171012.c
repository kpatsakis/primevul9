g_file_load_bytes_cb (GObject      *object,
                      GAsyncResult *result,
                      gpointer      user_data)
{
  GFile *file = G_FILE (object);
  GTask *task = user_data;
  GError *error = NULL;
  gchar *etag = NULL;
  gchar *contents = NULL;
  gsize len = 0;

  g_file_load_contents_finish (file, result, &contents, &len, &etag, &error);
  g_task_set_task_data (task, g_steal_pointer (&etag), g_free);

  if (error != NULL)
    g_task_return_error (task, g_steal_pointer (&error));
  else
    g_task_return_pointer (task,
                           g_bytes_new_take (g_steal_pointer (&contents), len),
                           (GDestroyNotify)g_bytes_unref);

  g_object_unref (task);
}