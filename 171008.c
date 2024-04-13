g_file_monitor_file (GFile              *file,
                     GFileMonitorFlags   flags,
                     GCancellable       *cancellable,
                     GError            **error)
{
  GFileIface *iface;
  GFileMonitor *monitor;

  g_return_val_if_fail (G_IS_FILE (file), NULL);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return NULL;

  iface = G_FILE_GET_IFACE (file);

  monitor = NULL;

  if (iface->monitor_file)
    monitor = (* iface->monitor_file) (file, flags, cancellable, NULL);

  /* Fallback to polling */
  if (monitor == NULL)
    monitor = _g_poll_file_monitor_new (file);

  return monitor;
}