g_file_mount_mountable (GFile               *file,
                        GMountMountFlags     flags,
                        GMountOperation     *mount_operation,
                        GCancellable        *cancellable,
                        GAsyncReadyCallback  callback,
                        gpointer             user_data)
{
  GFileIface *iface;

  g_return_if_fail (G_IS_FILE (file));

  iface = G_FILE_GET_IFACE (file);

  if (iface->mount_mountable == NULL)
    {
      g_task_report_new_error (file, callback, user_data,
                               g_file_mount_mountable,
                               G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                               _("Operation not supported"));
      return;
    }

  (* iface->mount_mountable) (file,
                              flags,
                              mount_operation,
                              cancellable,
                              callback,
                              user_data);
}