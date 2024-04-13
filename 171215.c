g_file_query_file_type (GFile               *file,
                        GFileQueryInfoFlags  flags,
                        GCancellable        *cancellable)
{
  GFileInfo *info;
  GFileType file_type;

  g_return_val_if_fail (G_IS_FILE(file), G_FILE_TYPE_UNKNOWN);
  info = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_TYPE, flags,
                            cancellable, NULL);
  if (info != NULL)
    {
      file_type = g_file_info_get_file_type (info);
      g_object_unref (info);
    }
  else
    file_type = G_FILE_TYPE_UNKNOWN;

  return file_type;
}