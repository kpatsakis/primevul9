_dbus_set_socket_nonblocking (DBusSocket      fd,
                              DBusError      *error)
{
  return _dbus_set_fd_nonblocking (fd.fd, error);
}