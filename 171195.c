g_file_real_measure_disk_usage (GFile                         *file,
                                GFileMeasureFlags              flags,
                                GCancellable                  *cancellable,
                                GFileMeasureProgressCallback   progress_callback,
                                gpointer                       progress_data,
                                guint64                       *disk_usage,
                                guint64                       *num_dirs,
                                guint64                       *num_files,
                                GError                       **error)
{
  g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                       "Operation not supported for the current backend.");
  return FALSE;
}