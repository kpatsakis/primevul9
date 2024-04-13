_dbus_write_socket_with_unix_fds(DBusSocket        fd,
                                 const DBusString *buffer,
                                 int               start,
                                 int               len,
                                 const int        *fds,
                                 int               n_fds) {

#ifndef HAVE_UNIX_FD_PASSING

  if (n_fds > 0) {
    errno = ENOTSUP;
    return -1;
  }

  return _dbus_write_socket(fd, buffer, start, len);
#else
  return _dbus_write_socket_with_unix_fds_two(fd, buffer, start, len, NULL, 0, 0, fds, n_fds);
#endif
}