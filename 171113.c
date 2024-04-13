g_file_move (GFile                  *source,
             GFile                  *destination,
             GFileCopyFlags          flags,
             GCancellable           *cancellable,
             GFileProgressCallback   progress_callback,
             gpointer                progress_callback_data,
             GError                **error)
{
  GFileIface *iface;
  GError *my_error;
  gboolean res;

  g_return_val_if_fail (G_IS_FILE (source), FALSE);
  g_return_val_if_fail (G_IS_FILE (destination), FALSE);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  iface = G_FILE_GET_IFACE (destination);
  if (iface->move)
    {
      my_error = NULL;
      res = (* iface->move) (source, destination,
                             flags, cancellable,
                             progress_callback, progress_callback_data,
                             &my_error);

      if (res)
        return TRUE;

      if (my_error->domain != G_IO_ERROR || my_error->code != G_IO_ERROR_NOT_SUPPORTED)
        {
          g_propagate_error (error, my_error);
          return FALSE;
        }
      else
        g_clear_error (&my_error);
    }

  /* If the types are different, and the destination method failed
   * also try the source method
   */
  if (G_OBJECT_TYPE (source) != G_OBJECT_TYPE (destination))
    {
      iface = G_FILE_GET_IFACE (source);

      if (iface->move)
        {
          my_error = NULL;
          res = (* iface->move) (source, destination,
                                 flags, cancellable,
                                 progress_callback, progress_callback_data,
                                 &my_error);

          if (res)
            return TRUE;

          if (my_error->domain != G_IO_ERROR || my_error->code != G_IO_ERROR_NOT_SUPPORTED)
            {
              g_propagate_error (error, my_error);
              return FALSE;
            }
          else
            g_clear_error (&my_error);
        }
    }

  if (flags & G_FILE_COPY_NO_FALLBACK_FOR_MOVE)
    {
      g_set_error_literal (error, G_IO_ERROR,
                           G_IO_ERROR_NOT_SUPPORTED,
                           _("Operation not supported"));
      return FALSE;
    }

  flags |= G_FILE_COPY_ALL_METADATA;
  if (!g_file_copy (source, destination, flags, cancellable,
                    progress_callback, progress_callback_data,
                    error))
    return FALSE;

  return g_file_delete (source, cancellable, error);
}