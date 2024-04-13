measure_disk_usage_thread (GTask        *task,
                           gpointer      source_object,
                           gpointer      task_data,
                           GCancellable *cancellable)
{
  MeasureTaskData *data = task_data;
  GError *error = NULL;
  MeasureResult result = { 0, };

  if (g_file_measure_disk_usage (source_object, data->flags, cancellable,
                                 data->progress_callback ? measure_disk_usage_progress : NULL, task,
                                 &result.disk_usage, &result.num_dirs, &result.num_files,
                                 &error))
    g_task_return_pointer (task, g_memdup (&result, sizeof result), g_free);
  else
    g_task_return_error (task, error);
}