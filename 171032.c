g_file_set_attribute (GFile                *file,
                      const gchar          *attribute,
                      GFileAttributeType    type,
                      gpointer              value_p,
                      GFileQueryInfoFlags   flags,
                      GCancellable         *cancellable,
                      GError              **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (attribute != NULL && *attribute != '\0', FALSE);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  iface = G_FILE_GET_IFACE (file);

  if (iface->set_attribute == NULL)
    {
      g_set_error_literal (error, G_IO_ERROR,
                           G_IO_ERROR_NOT_SUPPORTED,
                           _("Operation not supported"));
      return FALSE;
    }

  return (* iface->set_attribute) (file, attribute, type, value_p, flags, cancellable, error);
}