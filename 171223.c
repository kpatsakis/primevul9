g_file_eject_mountable (GFile               *file,
                        GMountUnmountFlags   flags,
                        GCancellable        *cancellable,
                        GAsyncReadyCallback  callback,
                        gpointer             user_data)
{
  GFileIface *iface;

  g_return_if_fail (G_IS_FILE (file));

  iface = G_FILE_GET_IFACE (file);

  if (iface->eject_mountable == NULL)
    {
      g_task_report_new_error (file, callback, user_data,
                               g_file_eject_mountable_with_operation,
                               G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                               _("Operation not supported"));
      return;
    }

  (* iface->eject_mountable) (file,
                              flags,
                              cancellable,
                              callback,
                              user_data);
}