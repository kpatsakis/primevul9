_dbus_socketpair (DBusSocket *fd1,
                  DBusSocket *fd2,
                  dbus_bool_t blocking,
                  DBusError  *error)
{
#ifdef HAVE_SOCKETPAIR
  int fds[2];
  int retval;

#ifdef SOCK_CLOEXEC
  dbus_bool_t cloexec_done;

  retval = socketpair(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0, fds);
  cloexec_done = retval >= 0;

  if (retval < 0 && (errno == EINVAL || errno == EPROTOTYPE))
#endif
    {
      retval = socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    }

  if (retval < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Could not create full-duplex pipe");
      return FALSE;
    }

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

#ifdef SOCK_CLOEXEC
  if (!cloexec_done)
#endif
    {
      _dbus_fd_set_close_on_exec (fds[0]);
      _dbus_fd_set_close_on_exec (fds[1]);
    }

  if (!blocking &&
      (!_dbus_set_fd_nonblocking (fds[0], NULL) ||
       !_dbus_set_fd_nonblocking (fds[1], NULL)))
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Could not set full-duplex pipe nonblocking");

      _dbus_close (fds[0], NULL);
      _dbus_close (fds[1], NULL);

      return FALSE;
    }

  fd1->fd = fds[0];
  fd2->fd = fds[1];

  _dbus_verbose ("full-duplex pipe %d <-> %d\n",
                 fd1->fd, fd2->fd);

  return TRUE;
#else
  _dbus_warn ("_dbus_socketpair() not implemented on this OS");
  dbus_set_error (error, DBUS_ERROR_FAILED,
                  "_dbus_socketpair() not implemented on this OS");
  return FALSE;
#endif
}