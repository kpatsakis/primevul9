file_get_target_path (GFile *file)
{
  GFileInfo *info;
  const char *target;
  char *path;

  info = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI, G_FILE_QUERY_INFO_NONE, NULL, NULL);
  if (info == NULL)
    return NULL;
  target = g_file_info_get_attribute_string (info, G_FILE_ATTRIBUTE_STANDARD_TARGET_URI);
  path = g_filename_from_uri (target, NULL, NULL);
  g_object_unref (info);

  return path;
}