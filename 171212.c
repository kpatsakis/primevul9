g_file_query_exists (GFile        *file,
                     GCancellable *cancellable)
{
  GFileInfo *info;

  g_return_val_if_fail (G_IS_FILE(file), FALSE);

  info = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_TYPE,
                            G_FILE_QUERY_INFO_NONE, cancellable, NULL);
  if (info != NULL)
    {
      g_object_unref (info);
      return TRUE;
    }

  return FALSE;
}