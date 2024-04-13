_dbus_write_socket (DBusSocket        fd,
                    const DBusString *buffer,
                    int               start,
                    int               len)
{
#if HAVE_DECL_MSG_NOSIGNAL
  const char *data;
  int bytes_written;

  data = _dbus_string_get_const_data_len (buffer, start, len);

 again:

  bytes_written = send (fd.fd, data, len, MSG_NOSIGNAL);

  if (bytes_written < 0 && errno == EINTR)
    goto again;

  return bytes_written;

#else
  return _dbus_write (fd.fd, buffer, start, len);
#endif
}