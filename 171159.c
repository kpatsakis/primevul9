do_splice (int     fd_in,
           loff_t *off_in,
           int     fd_out,
           loff_t *off_out,
           size_t  len,
           long   *bytes_transferd,
           GError **error)
{
  long result;

retry:
  result = splice (fd_in, off_in, fd_out, off_out, len, SPLICE_F_MORE);

  if (result == -1)
    {
      int errsv = errno;

      if (errsv == EINTR)
        goto retry;
      else if (errsv == ENOSYS || errsv == EINVAL || errsv == EOPNOTSUPP)
        g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_NOT_SUPPORTED,
                             _("Splice not supported"));
      else
        g_set_error (error, G_IO_ERROR,
                     g_io_error_from_errno (errsv),
                     _("Error splicing file: %s"),
                     g_strerror (errsv));

      return FALSE;
    }

  *bytes_transferd = result;
  return TRUE;
}