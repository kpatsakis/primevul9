_dbus_dup(int        fd,
          DBusError *error)
{
  int new_fd;

#ifdef F_DUPFD_CLOEXEC
  dbus_bool_t cloexec_done;

  new_fd = fcntl(fd, F_DUPFD_CLOEXEC, 3);
  cloexec_done = new_fd >= 0;

  if (new_fd < 0 && errno == EINVAL)
#endif
    {
      new_fd = fcntl(fd, F_DUPFD, 3);
    }

  if (new_fd < 0) {

    dbus_set_error (error, _dbus_error_from_errno (errno),
                    "Could not duplicate fd %d", fd);
    return -1;
  }

#ifdef F_DUPFD_CLOEXEC
  if (!cloexec_done)
#endif
    {
      _dbus_fd_set_close_on_exec(new_fd);
    }

  return new_fd;
}