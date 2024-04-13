g_file_copy_async (GFile                  *source,
                   GFile                  *destination,
                   GFileCopyFlags          flags,
                   int                     io_priority,
                   GCancellable           *cancellable,
                   GFileProgressCallback   progress_callback,
                   gpointer                progress_callback_data,
                   GAsyncReadyCallback     callback,
                   gpointer                user_data)
{
  GFileIface *iface;

  g_return_if_fail (G_IS_FILE (source));
  g_return_if_fail (G_IS_FILE (destination));

  iface = G_FILE_GET_IFACE (source);
  (* iface->copy_async) (source,
                         destination,
                         flags,
                         io_priority,
                         cancellable,
                         progress_callback,
                         progress_callback_data,
                         callback,
                         user_data);
}