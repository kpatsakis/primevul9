_dbus_listen_systemd_sockets (DBusSocket **fds,
                              DBusError   *error)
{
#ifdef HAVE_SYSTEMD
  int r, n;
  int fd;
  DBusSocket *new_fds;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  n = sd_listen_fds (TRUE);
  if (n < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (-n),
                      "Failed to acquire systemd socket: %s",
                      _dbus_strerror (-n));
      return -1;
    }

  if (n <= 0)
    {
      dbus_set_error (error, DBUS_ERROR_BAD_ADDRESS,
                      "No socket received.");
      return -1;
    }

  for (fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + n; fd ++)
    {
      r = sd_is_socket (fd, AF_UNSPEC, SOCK_STREAM, 1);
      if (r < 0)
        {
          dbus_set_error (error, _dbus_error_from_errno (-r),
                          "Failed to verify systemd socket type: %s",
                          _dbus_strerror (-r));
          return -1;
        }

      if (!r)
        {
          dbus_set_error (error, DBUS_ERROR_BAD_ADDRESS,
                          "Passed socket has wrong type.");
          return -1;
        }
    }

  /* OK, the file descriptors are all good, so let's take posession of
     them then. */

  new_fds = dbus_new (DBusSocket, n);
  if (!new_fds)
    {
      dbus_set_error (error, DBUS_ERROR_NO_MEMORY,
                      "Failed to allocate file handle array.");
      goto fail;
    }

  for (fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + n; fd ++)
    {
      if (!_dbus_set_fd_nonblocking (fd, error))
        {
          _DBUS_ASSERT_ERROR_IS_SET (error);
          goto fail;
        }

      new_fds[fd - SD_LISTEN_FDS_START].fd = fd;
    }

  *fds = new_fds;
  return n;

 fail:

  for (fd = SD_LISTEN_FDS_START; fd < SD_LISTEN_FDS_START + n; fd ++)
    {
      _dbus_close (fd, NULL);
    }

  dbus_free (new_fds);
  return -1;
#else
  dbus_set_error_const (error, DBUS_ERROR_NOT_SUPPORTED,
                        "dbus was compiled without systemd support");
  return -1;
#endif
}