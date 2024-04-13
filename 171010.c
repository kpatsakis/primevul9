g_file_replace_contents_bytes_async  (GFile               *file,
                                      GBytes              *contents,
                                      const char          *etag,
                                      gboolean             make_backup,
                                      GFileCreateFlags     flags,
                                      GCancellable        *cancellable,
                                      GAsyncReadyCallback  callback,
                                      gpointer             user_data)
{
  ReplaceContentsData *data;

  g_return_if_fail (G_IS_FILE (file));
  g_return_if_fail (contents != NULL);

  data = g_new0 (ReplaceContentsData, 1);

  data->content = g_bytes_ref (contents);

  data->task = g_task_new (file, cancellable, callback, user_data);
  g_task_set_source_tag (data->task, g_file_replace_contents_bytes_async);
  g_task_set_task_data (data->task, data, (GDestroyNotify)replace_contents_data_free);

  g_file_replace_async (file,
                        etag,
                        make_backup,
                        flags,
                        0,
                        g_task_get_cancellable (data->task),
                        replace_contents_open_callback,
                        data);
}