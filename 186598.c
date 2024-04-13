_dbus_write_two (int               fd,
                 const DBusString *buffer1,
                 int               start1,
                 int               len1,
                 const DBusString *buffer2,
                 int               start2,
                 int               len2)
{
  _dbus_assert (buffer1 != NULL);
  _dbus_assert (start1 >= 0);
  _dbus_assert (start2 >= 0);
  _dbus_assert (len1 >= 0);
  _dbus_assert (len2 >= 0);

#ifdef HAVE_WRITEV
  {
    struct iovec vectors[2];
    const char *data1;
    const char *data2;
    int bytes_written;

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

  again:

    bytes_written = writev (fd,
                            vectors,
                            data2 ? 2 : 1);

    if (bytes_written < 0 && errno == EINTR)
      goto again;

    return bytes_written;
  }
#else /* HAVE_WRITEV */
  {
    int ret1, ret2;

    ret1 = _dbus_write (fd, buffer1, start1, len1);
    if (ret1 == len1 && buffer2 != NULL)
      {
        ret2 = _dbus_write (fd, buffer2, start2, len2);
        if (ret2 < 0)
          ret2 = 0; /* we can't report an error as the first write was OK */

        return ret1 + ret2;
      }
    else
      return ret1;
  }
#endif /* !HAVE_WRITEV */
}