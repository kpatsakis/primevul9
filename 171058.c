g_file_copy (GFile                  *source,
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
  if (iface->copy)
    {
      my_error = NULL;
      res = (* iface->copy) (source, destination,
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

      if (iface->copy)
        {
          my_error = NULL;
          res = (* iface->copy) (source, destination,
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

  return file_copy_fallback (source, destination, flags, cancellable,
                             progress_callback, progress_callback_data,
                             error);
}