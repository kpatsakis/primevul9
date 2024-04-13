g_file_set_display_name (GFile         *file,
                         const gchar   *display_name,
                         GCancellable  *cancellable,
                         GError       **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (display_name != NULL, NULL);

  if (strchr (display_name, G_DIR_SEPARATOR) != NULL)
    {
      g_set_error (error,
                   G_IO_ERROR,
                   G_IO_ERROR_INVALID_ARGUMENT,
                   _("File names cannot contain “%c”"), G_DIR_SEPARATOR);
      return NULL;
    }

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return NULL;

  iface = G_FILE_GET_IFACE (file);

  return (* iface->set_display_name) (file, display_name, cancellable, error);
}