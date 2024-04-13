_dbus_close (int        fd,
             DBusError *error)
{
  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

 again:
  if (close (fd) < 0)
    {
      if (errno == EINTR)
        goto again;

      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Could not close fd %d", fd);
      return FALSE;
    }

  return TRUE;
}