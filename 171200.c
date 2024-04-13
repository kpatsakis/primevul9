g_file_eject_mountable_with_operation (GFile               *file,
                                       GMountUnmountFlags   flags,
                                       GMountOperation     *mount_operation,
                                       GCancellable        *cancellable,
                                       GAsyncReadyCallback  callback,
                                       gpointer             user_data)
{
  GFileIface *iface;

  g_return_if_fail (G_IS_FILE (file));

  iface = G_FILE_GET_IFACE (file);

  if (iface->eject_mountable == NULL && iface->eject_mountable_with_operation == NULL)
    {
      g_task_report_new_error (file, callback, user_data,
                               g_file_eject_mountable_with_operation,
                               G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                               _("Operation not supported"));
      return;
    }

  if (iface->eject_mountable_with_operation != NULL)
    (* iface->eject_mountable_with_operation) (file,
                                               flags,
                                               mount_operation,
                                               cancellable,
                                               callback,
                                               user_data);
  else
    (* iface->eject_mountable) (file,
                                flags,
                                cancellable,
                                callback,
                                user_data);
}