btrfs_reflink_with_progress (GInputStream           *in,
                             GOutputStream          *out,
                             GFileInfo              *info,
                             GCancellable           *cancellable,
                             GFileProgressCallback   progress_callback,
                             gpointer                progress_callback_data,
                             GError                **error)
{
  goffset source_size;
  int fd_in, fd_out;
  int ret, errsv;

  fd_in = g_file_descriptor_based_get_fd (G_FILE_DESCRIPTOR_BASED (in));
  fd_out = g_file_descriptor_based_get_fd (G_FILE_DESCRIPTOR_BASED (out));

  if (progress_callback)
    source_size = g_file_info_get_size (info);

  /* Btrfs clone ioctl properties:
   *  - Works at the inode level
   *  - Doesn't work with directories
   *  - Always follows symlinks (source and destination)
   *
   * By the time we get here, *in and *out are both regular files */
  ret = ioctl (fd_out, BTRFS_IOC_CLONE, fd_in);
  errsv = errno;

  if (ret < 0)
    {
      if (errsv == EXDEV)
	g_set_error_literal (error, G_IO_ERROR,
			     G_IO_ERROR_NOT_SUPPORTED,
			     _("Copy (reflink/clone) between mounts is not supported"));
      else if (errsv == EINVAL)
	g_set_error_literal (error, G_IO_ERROR,
			     G_IO_ERROR_NOT_SUPPORTED,
			     _("Copy (reflink/clone) is not supported or invalid"));
      else
	/* Most probably something odd happened; retry with fallback */
	g_set_error_literal (error, G_IO_ERROR,
			     G_IO_ERROR_NOT_SUPPORTED,
			     _("Copy (reflink/clone) is not supported or didn’t work"));
      /* We retry with fallback for all error cases because Btrfs is currently
       * unstable, and so we can't trust it to do clone properly.
       * In addition, any hard errors here would cause the same failure in the
       * fallback manual copy as well. */
      return FALSE;
    }

  /* Make sure we send full copied size */
  if (progress_callback)
    progress_callback (source_size, source_size, progress_callback_data);

  return TRUE;
}