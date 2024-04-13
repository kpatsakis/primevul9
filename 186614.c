add_linux_security_label_to_credentials (int              client_fd,
                                         DBusCredentials *credentials)
{
#if defined(__linux__) && defined(SO_PEERSEC)
  DBusString buf;
  socklen_t len = 1024;
  dbus_bool_t oom = FALSE;

  if (!_dbus_string_init_preallocated (&buf, len) ||
      !_dbus_string_set_length (&buf, len))
    return FALSE;

  while (getsockopt (client_fd, SOL_SOCKET, SO_PEERSEC,
         _dbus_string_get_data (&buf), &len) < 0)
    {
      int e = errno;

      _dbus_verbose ("getsockopt failed with %s, len now %lu\n",
                     _dbus_strerror (e), (unsigned long) len);

      if (e != ERANGE || len <= _dbus_string_get_length_uint (&buf))
        {
          _dbus_verbose ("Failed to getsockopt(SO_PEERSEC): %s\n",
                         _dbus_strerror (e));
          goto out;
        }

      /* If not enough space, len is updated to be enough.
       * Try again with a large enough buffer. */
      if (!_dbus_string_set_length (&buf, len))
        {
          oom = TRUE;
          goto out;
        }

      _dbus_verbose ("will try again with %lu\n", (unsigned long) len);
    }

  if (len <= 0)
    {
      _dbus_verbose ("getsockopt(SO_PEERSEC) yielded <= 0 bytes: %lu\n",
                     (unsigned long) len);
      goto out;
    }

  if (len > _dbus_string_get_length_uint (&buf))
    {
      _dbus_verbose ("%lu > %u", (unsigned long) len,
                     _dbus_string_get_length_uint (&buf));
      _dbus_assert_not_reached ("getsockopt(SO_PEERSEC) overflowed");
    }

  if (_dbus_string_get_byte (&buf, len - 1) == 0)
    {
      /* the kernel included the trailing \0 in its count,
       * but DBusString always has an extra \0 after the data anyway */
      _dbus_verbose ("subtracting trailing \\0\n");
      len--;
    }

  if (!_dbus_string_set_length (&buf, len))
    {
      _dbus_assert_not_reached ("shortening string should not lead to OOM");
      oom = TRUE;
      goto out;
    }

  if (strlen (_dbus_string_get_const_data (&buf)) != len)
    {
      /* LSM people on the linux-security-module@ mailing list say this
       * should never happen: the label should be a bytestring with
       * an optional trailing \0 */
      _dbus_verbose ("security label from kernel had an embedded \\0, "
                     "ignoring it\n");
      goto out;
    }

  _dbus_verbose ("getsockopt(SO_PEERSEC): %lu bytes excluding \\0: %s\n",
                 (unsigned long) len,
                 _dbus_string_get_const_data (&buf));

  if (!_dbus_credentials_add_linux_security_label (credentials,
        _dbus_string_get_const_data (&buf)))
    {
      oom = TRUE;
      goto out;
    }

out:
  _dbus_string_free (&buf);
  return !oom;
#else
  /* no error */
  return TRUE;
#endif
}