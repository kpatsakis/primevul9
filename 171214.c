measure_disk_usage_progress (gboolean reporting,
                             guint64  current_size,
                             guint64  num_dirs,
                             guint64  num_files,
                             gpointer user_data)
{
  MeasureProgress progress;
  GTask *task = user_data;
  MeasureTaskData *data;

  data = g_task_get_task_data (task);

  progress.callback = data->progress_callback;
  progress.user_data = data->progress_data;
  progress.reporting = reporting;
  progress.current_size = current_size;
  progress.num_dirs = num_dirs;
  progress.num_files = num_files;

  g_main_context_invoke_full (g_task_get_context (task),
                              g_task_get_priority (task),
                              measure_disk_usage_invoke_progress,
                              g_memdup (&progress, sizeof progress),
                              g_free);
}