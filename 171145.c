g_file_enumerate_children_async (GFile               *file,
                                 const char          *attributes,
                                 GFileQueryInfoFlags  flags,
                                 int                  io_priority,
                                 GCancellable        *cancellable,
                                 GAsyncReadyCallback  callback,
                                 gpointer             user_data)
{
  GFileIface *iface;

  g_return_if_fail (G_IS_FILE (file));

  iface = G_FILE_GET_IFACE (file);
  (* iface->enumerate_children_async) (file,
                                       attributes,
                                       flags,
                                       io_priority,
                                       cancellable,
                                       callback,
                                       user_data);
}