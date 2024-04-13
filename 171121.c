g_file_real_measure_disk_usage_finish (GFile         *file,
                                       GAsyncResult  *result,
                                       guint64       *disk_usage,
                                       guint64       *num_dirs,
                                       guint64       *num_files,
                                       GError       **error)
{
  MeasureResult *measure_result;

  g_return_val_if_fail (g_task_is_valid (result, file), FALSE);

  measure_result = g_task_propagate_pointer (G_TASK (result), error);

  if (measure_result == NULL)
    return FALSE;

  if (disk_usage)
    *disk_usage = measure_result->disk_usage;

  if (num_dirs)
    *num_dirs = measure_result->num_dirs;

  if (num_files)
    *num_files = measure_result->num_files;

  g_free (measure_result);

  return TRUE;
}