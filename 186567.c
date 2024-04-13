_dbus_close_socket (DBusSocket        fd,
                    DBusError        *error)
{
  return _dbus_close (fd.fd, error);
}