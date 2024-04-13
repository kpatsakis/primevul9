g_file_set_display_name_async (GFile               *file,
                               const gchar         *display_name,
                               gint                 io_priority,
                               GCancellable        *cancellable,
                               GAsyncReadyCallback  callback,
                               gpointer             user_data)
{
  GFileIface *iface;

  g_return_if_fail (G_IS_FILE (file));
  g_return_if_fail (display_name != NULL);

  iface = G_FILE_GET_IFACE (file);
  (* iface->set_display_name_async) (file,
                                     display_name,
                                     io_priority,
                                     cancellable,
                                     callback,
                                     user_data);
}