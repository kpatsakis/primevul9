g_file_open_readwrite (GFile         *file,
                       GCancellable  *cancellable,
                       GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), NULL);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return NULL;

  iface = G_FILE_GET_IFACE (file);

  if (iface->open_readwrite == NULL)
    {
      g_set_error_literal (error, G_IO_ERROR,
                           G_IO_ERROR_NOT_SUPPORTED,
                           _("Operation not supported"));
      return NULL;
    }

  return (* iface->open_readwrite) (file, cancellable, error);
}