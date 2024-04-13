g_file_monitor (GFile              *file,
                GFileMonitorFlags   flags,
                GCancellable       *cancellable,
                GError            **error)
{
  if (g_file_query_file_type (file, 0, cancellable) == G_FILE_TYPE_DIRECTORY)
    return g_file_monitor_directory (file,
                                     flags & ~G_FILE_MONITOR_WATCH_HARD_LINKS,
                                     cancellable, error);
  else
    return g_file_monitor_file (file, flags, cancellable, error);
}