query_default_handler_query_info_cb (GObject      *object,
                                     GAsyncResult *result,
                                     gpointer      user_data)
{
  GFile *file = G_FILE (object);
  GTask *task = G_TASK (user_data);
  GError *error = NULL;
  GFileInfo *info;
  const char *content_type;
  GAppInfo *appinfo = NULL;

  info = g_file_query_info_finish (file, result, &error);
  if (info == NULL)
    {
      g_task_return_error (task, g_steal_pointer (&error));
      g_object_unref (task);
      return;
    }

  content_type = g_file_info_get_content_type (info);
  if (content_type)
    {
      char *path;

      /* Don't use is_native(), as we want to support fuse paths if available */
      path = g_file_get_path (file);

      /* FIXME: The following still uses blocking calls. */
      appinfo = g_app_info_get_default_for_type (content_type,
                                                 path == NULL);
      g_free (path);
    }

  g_object_unref (info);

  if (appinfo != NULL)
    g_task_return_pointer (task, g_steal_pointer (&appinfo), g_object_unref);
  else
    g_task_return_new_error (task,
                             G_IO_ERROR,
                             G_IO_ERROR_NOT_SUPPORTED,
                             _("No application is registered as handling this file"));
  g_object_unref (task);
}