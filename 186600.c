_dbus_generate_random_bytes (DBusString *str,
                             int         n_bytes,
                             DBusError  *error)
{
  int old_len = _dbus_string_get_length (str);
  int fd;
  int result;
#ifdef HAVE_GETRANDOM
  char *buffer;

  if (!_dbus_string_lengthen (str, n_bytes))
    {
      _DBUS_SET_OOM (error);
      return FALSE;
    }

  buffer = _dbus_string_get_data_len (str, old_len, n_bytes);
  result = getrandom (buffer, n_bytes, GRND_NONBLOCK);

  if (result == n_bytes)
    return TRUE;

  _dbus_string_set_length (str, old_len);
#endif

  /* note, urandom on linux will fall back to pseudorandom */
  fd = open ("/dev/urandom", O_RDONLY);

  if (fd < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Could not open /dev/urandom: %s",
                      _dbus_strerror (errno));
      return FALSE;
    }

  _dbus_verbose ("/dev/urandom fd %d opened\n", fd);

  result = _dbus_read (fd, str, n_bytes);

  if (result != n_bytes)
    {
      if (result < 0)
        dbus_set_error (error, _dbus_error_from_errno (errno),
                        "Could not read /dev/urandom: %s",
                        _dbus_strerror (errno));
      else
        dbus_set_error (error, DBUS_ERROR_IO_ERROR,
                        "Short read from /dev/urandom");

      _dbus_close (fd, NULL);
      _dbus_string_set_length (str, old_len);
      return FALSE;
    }

  _dbus_verbose ("Read %d bytes from /dev/urandom\n",
                 n_bytes);

  _dbus_close (fd, NULL);

  return TRUE;
}