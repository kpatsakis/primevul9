g_file_make_symbolic_link (GFile         *file,
                           const char    *symlink_value,
                           GCancellable  *cancellable,
                           GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (symlink_value != NULL, FALSE);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  if (*symlink_value == '\0')
    {
      g_set_error_literal (error, G_IO_ERROR,
                           G_IO_ERROR_INVALID_ARGUMENT,
                           _("Invalid symlink value given"));
      return FALSE;
    }

  iface = G_FILE_GET_IFACE (file);

  if (iface->make_symbolic_link == NULL)
    {
      g_set_error_literal (error, G_IO_ERROR,
                           G_IO_ERROR_NOT_SUPPORTED,
                           _("Operation not supported"));
      return FALSE;
    }

  return (* iface->make_symbolic_link) (file, symlink_value, cancellable, error);
}