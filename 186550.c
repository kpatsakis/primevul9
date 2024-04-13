_dbus_send_credentials_socket  (DBusSocket       server_fd,
                                DBusError       *error)
{
  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  if (write_credentials_byte (server_fd.fd, error))
    return TRUE;
  else
    return FALSE;
}