copy_symlink (GFile           *destination,
              GFileCopyFlags   flags,
              GCancellable    *cancellable,
              const char      *target,
              GError         **error)
{
  GError *my_error;
  gboolean tried_delete;
  GFileInfo *info;
  GFileType file_type;

  tried_delete = FALSE;

 retry:
  my_error = NULL;
  if (!g_file_make_symbolic_link (destination, target, cancellable, &my_error))
    {
      /* Maybe it already existed, and we want to overwrite? */
      if (!tried_delete && (flags & G_FILE_COPY_OVERWRITE) &&
          my_error->domain == G_IO_ERROR && my_error->code == G_IO_ERROR_EXISTS)
        {
          g_clear_error (&my_error);

          /* Don't overwrite if the destination is a directory */
          info = g_file_query_info (destination, G_FILE_ATTRIBUTE_STANDARD_TYPE,
                                    G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                                    cancellable, &my_error);
          if (info != NULL)
            {
              file_type = g_file_info_get_file_type (info);
              g_object_unref (info);

              if (file_type == G_FILE_TYPE_DIRECTORY)
                {
                  g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_IS_DIRECTORY,
                                       _("Can’t copy over directory"));
                  return FALSE;
                }
            }

          if (!g_file_delete (destination, cancellable, error))
            return FALSE;

          tried_delete = TRUE;
          goto retry;
        }
            /* Nah, fail */
      g_propagate_error (error, my_error);
      return FALSE;
    }

  return TRUE;
}