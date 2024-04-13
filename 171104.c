g_file_delete_async (GFile               *file,
                     int                  io_priority,
                     GCancellable        *cancellable,
                     GAsyncReadyCallback  callback,
                     gpointer             user_data)
{
  GFileIface *iface;

  g_return_if_fail (G_IS_FILE (file));

  iface = G_FILE_GET_IFACE (file);
  (* iface->delete_file_async) (file,
                                io_priority,
                                cancellable,
                                callback,
                                user_data);
}