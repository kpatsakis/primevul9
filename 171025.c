g_file_real_measure_disk_usage_async (GFile                        *file,
                                      GFileMeasureFlags             flags,
                                      gint                          io_priority,
                                      GCancellable                 *cancellable,
                                      GFileMeasureProgressCallback  progress_callback,
                                      gpointer                      progress_data,
                                      GAsyncReadyCallback           callback,
                                      gpointer                      user_data)
{
  MeasureTaskData data;
  GTask *task;

  data.flags = flags;
  data.progress_callback = progress_callback;
  data.progress_data = progress_data;

  task = g_task_new (file, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_file_real_measure_disk_usage_async);
  g_task_set_task_data (task, g_memdup (&data, sizeof data), g_free);
  g_task_set_priority (task, io_priority);

  g_task_run_in_thread (task, measure_disk_usage_thread);
  g_object_unref (task);
}