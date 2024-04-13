_dbus_write_socket_two (DBusSocket        fd,
                        const DBusString *buffer1,
                        int               start1,
                        int               len1,
                        const DBusString *buffer2,
                        int               start2,
                        int               len2)
{
#if HAVE_DECL_MSG_NOSIGNAL
  struct iovec vectors[2];
  const char *data1;
  const char *data2;
  int bytes_written;
  struct msghdr m;

  _dbus_assert (buffer1 != NULL);
  _dbus_assert (start1 >= 0);
  _dbus_assert (start2 >= 0);
  _dbus_assert (len1 >= 0);
  _dbus_assert (len2 >= 0);

  data1 = _dbus_string_get_const_data_len (buffer1, start1, len1);

  if (buffer2 != NULL)
    data2 = _dbus_string_get_const_data_len (buffer2, start2, len2);
  else
    {
      data2 = NULL;
      start2 = 0;
      len2 = 0;
    }

  vectors[0].iov_base = (char*) data1;
  vectors[0].iov_len = len1;
  vectors[1].iov_base = (char*) data2;
  vectors[1].iov_len = len2;

  _DBUS_ZERO(m);
  m.msg_iov = vectors;
  m.msg_iovlen = data2 ? 2 : 1;

 again:

  bytes_written = sendmsg (fd.fd, &m, MSG_NOSIGNAL);

  if (bytes_written < 0 && errno == EINTR)
    goto again;

  return bytes_written;

#else
  return _dbus_write_two (fd.fd, buffer1, start1, len1,
                          buffer2, start2, len2);
#endif
}