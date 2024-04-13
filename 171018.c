g_file_mount_enclosing_volume (GFile               *location,
                               GMountMountFlags     flags,
                               GMountOperation     *mount_operation,
                               GCancellable        *cancellable,
                               GAsyncReadyCallback  callback,
                               gpointer             user_data)
{
  GFileIface *iface;

  g_return_if_fail (G_IS_FILE (location));

  iface = G_FILE_GET_IFACE (location);

  if (iface->mount_enclosing_volume == NULL)
    {
      g_task_report_new_error (location, callback, user_data,
                               g_file_mount_enclosing_volume,
                               G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                               _("volume doesn’t implement mount"));
      return;
    }

  (* iface->mount_enclosing_volume) (location, flags, mount_operation, cancellable, callback, user_data);

}