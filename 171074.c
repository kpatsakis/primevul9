g_file_query_default_handler_async (GFile              *file,
                                    int                 io_priority,
                                    GCancellable       *cancellable,
                                    GAsyncReadyCallback callback,
                                    gpointer            user_data)
{
  GTask *task;
  char *uri_scheme;

  task = g_task_new (file, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_file_query_default_handler_async);

  uri_scheme = g_file_get_uri_scheme (file);
  if (uri_scheme && uri_scheme[0] != '\0')
    {
      GAppInfo *appinfo;

      /* FIXME: The following still uses blocking calls. */
      appinfo = g_app_info_get_default_for_uri_scheme (uri_scheme);
      g_free (uri_scheme);

      if (appinfo != NULL)
        {
          g_task_return_pointer (task, g_steal_pointer (&appinfo), g_object_unref);
          g_object_unref (task);
          return;
        }
    }
  else
    g_free (uri_scheme);

  g_file_query_info_async (file,
                           G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
                           0,
                           io_priority,
                           cancellable,
                           query_default_handler_query_info_cb,
                           g_steal_pointer (&task));
}