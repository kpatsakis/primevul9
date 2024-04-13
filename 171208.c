g_file_measure_disk_usage (GFile                         *file,
                           GFileMeasureFlags              flags,
                           GCancellable                  *cancellable,
                           GFileMeasureProgressCallback   progress_callback,
                           gpointer                       progress_data,
                           guint64                       *disk_usage,
                           guint64                       *num_dirs,
                           guint64                       *num_files,
                           GError                       **error)
{
  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  return G_FILE_GET_IFACE (file)->measure_disk_usage (file, flags, cancellable,
                                                      progress_callback, progress_data,
                                                      disk_usage, num_dirs, num_files,
                                                      error);
}