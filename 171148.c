g_file_measure_disk_usage_async (GFile                        *file,
                                 GFileMeasureFlags             flags,
                                 gint                          io_priority,
                                 GCancellable                 *cancellable,
                                 GFileMeasureProgressCallback  progress_callback,
                                 gpointer                      progress_data,
                                 GAsyncReadyCallback           callback,
                                 gpointer                      user_data)
{
  g_return_if_fail (G_IS_FILE (file));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

  G_FILE_GET_IFACE (file)->measure_disk_usage_async (file, flags, io_priority, cancellable,
                                                     progress_callback, progress_data,
                                                     callback, user_data);
}