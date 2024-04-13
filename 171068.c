g_file_measure_disk_usage_finish (GFile         *file,
                                  GAsyncResult  *result,
                                  guint64       *disk_usage,
                                  guint64       *num_dirs,
                                  guint64       *num_files,
                                  GError       **error)
{
  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  return G_FILE_GET_IFACE (file)->measure_disk_usage_finish (file, result, disk_usage, num_dirs, num_files, error);
}