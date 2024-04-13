_dbus_read_socket (DBusSocket        fd,
                   DBusString       *buffer,
                   int               count)
{
  return _dbus_read (fd.fd, buffer, count);
}