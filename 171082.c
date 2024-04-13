g_file_replace_async (GFile               *file,
                      const char          *etag,
                      gboolean             make_backup,
                      GFileCreateFlags     flags,
                      int                  io_priority,
                      GCancellable        *cancellable,
                      GAsyncReadyCallback  callback,
                      gpointer             user_data)
{
  GFileIface *iface;

  g_return_if_fail (G_IS_FILE (file));

  iface = G_FILE_GET_IFACE (file);
  (* iface->replace_async) (file,
                            etag,
                            make_backup,
                            flags,
                            io_priority,
                            cancellable,
                            callback,
                            user_data);
}