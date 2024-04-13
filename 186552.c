_dbus_open_unix_socket (int              *fd,
                        DBusError        *error)
{
  return _dbus_open_socket(fd, PF_UNIX, SOCK_STREAM, 0, error);
}