splice_stream_with_progress (GInputStream           *in,
                             GOutputStream          *out,
                             GCancellable           *cancellable,
                             GFileProgressCallback   progress_callback,
                             gpointer                progress_callback_data,
                             GError                **error)
{
  int buffer[2] = { -1, -1 };
  int buffer_size;
  gboolean res;
  goffset total_size;
  loff_t offset_in;
  loff_t offset_out;
  int fd_in, fd_out;

  fd_in = g_file_descriptor_based_get_fd (G_FILE_DESCRIPTOR_BASED (in));
  fd_out = g_file_descriptor_based_get_fd (G_FILE_DESCRIPTOR_BASED (out));

  if (!g_unix_open_pipe (buffer, FD_CLOEXEC, error))
    return FALSE;

#if defined(F_SETPIPE_SZ) && defined(F_GETPIPE_SZ)
  /* Try a 1MiB buffer for improved throughput. If that fails, use the default
   * pipe size. See: https://bugzilla.gnome.org/791457 */
  buffer_size = fcntl (buffer[1], F_SETPIPE_SZ, 1024 * 1024);
  if (buffer_size <= 0)
    {
      int errsv;
      buffer_size = fcntl (buffer[1], F_GETPIPE_SZ);
      errsv = errno;

      if (buffer_size <= 0)
        {
          g_set_error (error, G_IO_ERROR, g_io_error_from_errno (errsv),
                       _("Error splicing file: %s"), g_strerror (errsv));
          res = FALSE;
          goto out;
        }
    }
#else
  /* If #F_GETPIPE_SZ isn’t available, assume we’re on Linux < 2.6.35,
   * but ≥ 2.6.11, meaning the pipe capacity is 64KiB. Ignore the possibility of
   * running on Linux < 2.6.11 (where the capacity was the system page size,
   * typically 4KiB) because it’s ancient. See pipe(7). */
  buffer_size = 1024 * 64;
#endif

  g_assert (buffer_size > 0);

  total_size = -1;
  /* avoid performance impact of querying total size when it's not needed */
  if (progress_callback)
    {
      struct stat sbuf;

      if (fstat (fd_in, &sbuf) == 0)
        total_size = sbuf.st_size;
    }

  if (total_size == -1)
    total_size = 0;

  offset_in = offset_out = 0;
  res = FALSE;
  while (TRUE)
    {
      long n_read;
      long n_written;

      if (g_cancellable_set_error_if_cancelled (cancellable, error))
        break;

      if (!do_splice (fd_in, &offset_in, buffer[1], NULL, buffer_size, &n_read, error))
        break;

      if (n_read == 0)
        {
          res = TRUE;
          break;
        }

      while (n_read > 0)
        {
          if (g_cancellable_set_error_if_cancelled (cancellable, error))
            goto out;

          if (!do_splice (buffer[0], NULL, fd_out, &offset_out, n_read, &n_written, error))
            goto out;

          n_read -= n_written;
        }

      if (progress_callback)
        progress_callback (offset_in, total_size, progress_callback_data);
    }

  /* Make sure we send full copied size */
  if (progress_callback)
    progress_callback (offset_in, total_size, progress_callback_data);

  if (!g_close (buffer[0], error))
    goto out;
  buffer[0] = -1;
  if (!g_close (buffer[1], error))
    goto out;
  buffer[1] = -1;
 out:
  if (buffer[0] != -1)
    (void) g_close (buffer[0], NULL);
  if (buffer[1] != -1)
    (void) g_close (buffer[1], NULL);

  return res;
}