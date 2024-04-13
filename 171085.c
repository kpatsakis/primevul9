g_file_real_copy_async (GFile                  *source,
                        GFile                  *destination,
                        GFileCopyFlags          flags,
                        int                     io_priority,
                        GCancellable           *cancellable,
                        GFileProgressCallback   progress_callback,
                        gpointer                progress_callback_data,
                        GAsyncReadyCallback     callback,
                        gpointer                user_data)
{
  GTask *task;
  CopyAsyncData *data;

  data = g_slice_new (CopyAsyncData);
  data->source = g_object_ref (source);
  data->destination = g_object_ref (destination);
  data->flags = flags;
  data->progress_cb = progress_callback;
  data->progress_cb_data = progress_callback_data;

  task = g_task_new (source, cancellable, callback, user_data);
  g_task_set_source_tag (task, g_file_real_copy_async);
  g_task_set_task_data (task, data, (GDestroyNotify)copy_async_data_free);
  g_task_set_priority (task, io_priority);
  g_task_run_in_thread (task, copy_async_thread);
  g_object_unref (task);
}