g_file_find_enclosing_mount (GFile         *file,
                             GCancellable  *cancellable,
                             GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), NULL);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return NULL;

  iface = G_FILE_GET_IFACE (file);
  if (iface->find_enclosing_mount == NULL)
    {

      g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
                           /* Translators: This is an error message when
                            * trying to find the enclosing (user visible)
                            * mount of a file, but none exists.
                            */
                           _("Containing mount does not exist"));
      return NULL;
    }

  return (* iface->find_enclosing_mount) (file, cancellable, error);
}