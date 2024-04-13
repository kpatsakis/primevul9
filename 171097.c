g_file_set_attributes_from_info (GFile                *file,
                                 GFileInfo            *info,
                                 GFileQueryInfoFlags   flags,
                                 GCancellable         *cancellable,
                                 GError              **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (G_IS_FILE_INFO (info), FALSE);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  g_file_info_clear_status (info);

  iface = G_FILE_GET_IFACE (file);

  return (* iface->set_attributes_from_info) (file,
                                              info,
                                              flags,
                                              cancellable,
                                              error);
}