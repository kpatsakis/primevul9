copy_async_progress_callback (goffset  current_num_bytes,
                              goffset  total_num_bytes,
                              gpointer user_data)
{
  GTask *task = user_data;
  CopyAsyncData *data = g_task_get_task_data (task);
  ProgressData *progress;

  progress = g_new (ProgressData, 1);
  progress->data = data;
  progress->current_num_bytes = current_num_bytes;
  progress->total_num_bytes = total_num_bytes;

  g_main_context_invoke_full (g_task_get_context (task),
                              g_task_get_priority (task),
                              copy_async_progress_in_main,
                              progress,
                              g_free);
}