open_source_for_copy (GFile           *source,
                      GFile           *destination,
                      GFileCopyFlags   flags,
                      GCancellable    *cancellable,
                      GError         **error)
{
  GError *my_error;
  GFileInputStream *ret;
  GFileInfo *info;
  GFileType file_type;

  my_error = NULL;
  ret = g_file_read (source, cancellable, &my_error);
  if (ret != NULL)
    return ret;

  /* There was an error opening the source, try to set a good error for it: */
  if (my_error->domain == G_IO_ERROR && my_error->code == G_IO_ERROR_IS_DIRECTORY)
    {
      /* The source is a directory, don't fail with WOULD_RECURSE immediately,
       * as that is less useful to the app. Better check for errors on the
       * target instead.
       */
      g_error_free (my_error);
      my_error = NULL;

      info = g_file_query_info (destination, G_FILE_ATTRIBUTE_STANDARD_TYPE,
                                G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                cancellable, &my_error);
      if (info != NULL &&
          g_file_info_has_attribute (info, G_FILE_ATTRIBUTE_STANDARD_TYPE))
        {
          file_type = g_file_info_get_file_type (info);
          g_object_unref (info);

          if (flags & G_FILE_COPY_OVERWRITE)
            {
              if (file_type == G_FILE_TYPE_DIRECTORY)
                {
                  g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_WOULD_MERGE,
                                       _("Can’t copy directory over directory"));
                  return NULL;
                }
              /* continue to would_recurse error */
            }
          else
            {
              g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_EXISTS,
                                   _("Target file exists"));
              return NULL;
            }
        }
      else
        {
          /* Error getting info from target, return that error
           * (except for NOT_FOUND, which is no error here)
           */
          g_clear_object (&info);
          if (my_error != NULL && !g_error_matches (my_error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND))
            {
              g_propagate_error (error, my_error);
              return NULL;
            }
          g_clear_error (&my_error);
        }

      g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_WOULD_RECURSE,
                           _("Can’t recursively copy directory"));
      return NULL;
    }

  g_propagate_error (error, my_error);
  return NULL;
}