g_file_append_to_async (GFile               *file,
                        GFileCreateFlags     flags,
                        int                  io_priority,
                        GCancellable        *cancellable,
                        GAsyncReadyCallback  callback,
                        gpointer             user_data)
{
  GFileIface *iface;

  g_return_if_fail (G_IS_FILE (file));

  iface = G_FILE_GET_IFACE (file);
  (* iface->append_to_async) (file,
                              flags,
                              io_priority,
                              cancellable,
                              callback,
                              user_data);
}