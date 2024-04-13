g_file_delete (GFile         *file,
               GCancellable  *cancellable,
               GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  iface = G_FILE_GET_IFACE (file);

  if (iface->delete_file == NULL)
    {
      g_set_error_literal (error, G_IO_ERROR,
                           G_IO_ERROR_NOT_SUPPORTED,
                           _("Operation not supported"));
      return FALSE;
    }

  return (* iface->delete_file) (file, cancellable, error);
}