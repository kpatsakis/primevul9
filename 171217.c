g_file_replace (GFile             *file,
                const char        *etag,
                gboolean           make_backup,
                GFileCreateFlags   flags,
                GCancellable      *cancellable,
                GError           **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), NULL);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return NULL;

  iface = G_FILE_GET_IFACE (file);

  if (iface->replace == NULL)
    {
      g_set_error_literal (error, G_IO_ERROR,
                           G_IO_ERROR_NOT_SUPPORTED,
                           _("Operation not supported"));
      return NULL;
    }

  /* Handle empty tag string as NULL in consistent way. */
  if (etag && *etag == 0)
    etag = NULL;

  return (* iface->replace) (file, etag, make_backup, flags, cancellable, error);
}