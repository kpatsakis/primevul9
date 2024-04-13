g_file_query_filesystem_info_async (GFile               *file,
                                    const char          *attributes,
                                    int                  io_priority,
                                    GCancellable        *cancellable,
                                    GAsyncReadyCallback  callback,
                                    gpointer             user_data)
{
  GFileIface *iface;

  g_return_if_fail (G_IS_FILE (file));

  iface = G_FILE_GET_IFACE (file);
  (* iface->query_filesystem_info_async) (file,
                                          attributes,
                                          io_priority,
                                          cancellable,
                                          callback,
                                          user_data);
}