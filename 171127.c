g_file_monitor_directory (GFile              *file,
                          GFileMonitorFlags   flags,
                          GCancellable       *cancellable,
                          GError            **error)
{
  GFileIface *iface;

  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (~flags & G_FILE_MONITOR_WATCH_HARD_LINKS, NULL);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return NULL;

  iface = G_FILE_GET_IFACE (file);

  if (iface->monitor_dir == NULL)
    {
      g_set_error_literal (error, G_IO_ERROR,
                           G_IO_ERROR_NOT_SUPPORTED,
                           _("Operation not supported"));
      return NULL;
    }

  return (* iface->monitor_dir) (file, flags, cancellable, error);
}