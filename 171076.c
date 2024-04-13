g_file_copy_attributes (GFile           *source,
                        GFile           *destination,
                        GFileCopyFlags   flags,
                        GCancellable    *cancellable,
                        GError         **error)
{
  char *attrs_to_read;
  gboolean res;
  GFileInfo *info;
  gboolean source_nofollow_symlinks;

  if (!build_attribute_list_for_copy (destination, flags, &attrs_to_read,
                                      cancellable, error))
    return FALSE;

  source_nofollow_symlinks = flags & G_FILE_COPY_NOFOLLOW_SYMLINKS;

  /* Ignore errors here, if we can't read some info (e.g. if it doesn't exist)
   * we just don't copy it.
   */
  info = g_file_query_info (source, attrs_to_read,
                            source_nofollow_symlinks ? G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS:0,
                            cancellable,
                            NULL);

  g_free (attrs_to_read);

  res = TRUE;
  if  (info)
    {
      res = g_file_set_attributes_from_info (destination,
                                             info,
                                             G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                             cancellable,
                                             error);
      g_object_unref (info);
    }

  return res;
}