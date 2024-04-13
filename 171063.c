g_file_set_attributes_async (GFile               *file,
                             GFileInfo           *info,
                             GFileQueryInfoFlags  flags,
                             int                  io_priority,
                             GCancellable        *cancellable,
                             GAsyncReadyCallback  callback,
                             gpointer             user_data)
{
  GFileIface *iface;

  g_return_if_fail (G_IS_FILE (file));
  g_return_if_fail (G_IS_FILE_INFO (info));

  iface = G_FILE_GET_IFACE (file);
  (* iface->set_attributes_async) (file,
                                   info,
                                   flags,
                                   io_priority,
                                   cancellable,
                                   callback,
                                   user_data);
}