g_file_query_default_handler (GFile         *file,
                              GCancellable  *cancellable,
                              GError       **error)
{
  char *uri_scheme;
  const char *content_type;
  GAppInfo *appinfo;
  GFileInfo *info;
  char *path;

  uri_scheme = g_file_get_uri_scheme (file);
  if (uri_scheme && uri_scheme[0] != '\0')
    {
      appinfo = g_app_info_get_default_for_uri_scheme (uri_scheme);
      g_free (uri_scheme);

      if (appinfo != NULL)
        return appinfo;
    }
  else
    g_free (uri_scheme);

  info = g_file_query_info (file,
                            G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
                            0,
                            cancellable,
                            error);
  if (info == NULL)
    return NULL;

  appinfo = NULL;

  content_type = g_file_info_get_content_type (info);
  if (content_type)
    {
      /* Don't use is_native(), as we want to support fuse paths if available */
      path = g_file_get_path (file);
      appinfo = g_app_info_get_default_for_type (content_type,
                                                 path == NULL);
      g_free (path);
    }

  g_object_unref (info);

  if (appinfo != NULL)
    return appinfo;

  g_set_error_literal (error, G_IO_ERROR,
                       G_IO_ERROR_NOT_SUPPORTED,
                       _("No application is registered as handling this file"));
  return NULL;
}