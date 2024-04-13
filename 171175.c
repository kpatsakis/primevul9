g_file_load_contents_async (GFile               *file,
                           GCancellable        *cancellable,
                           GAsyncReadyCallback  callback,
                           gpointer             user_data)
{
  g_file_load_partial_contents_async (file,
                                      cancellable,
                                      NULL,
                                      callback, user_data);
}