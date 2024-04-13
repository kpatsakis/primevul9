_dbus_lookup_session_address_launchd (DBusString *address, DBusError  *error)
{
  dbus_bool_t valid_socket;
  DBusString socket_path;

  if (_dbus_check_setuid ())
    {
      dbus_set_error_const (error, DBUS_ERROR_NOT_SUPPORTED,
                            "Unable to find launchd socket when setuid");
      return FALSE;
    }

  if (!_dbus_string_init (&socket_path))
    {
      _DBUS_SET_OOM (error);
      return FALSE;
    }

  valid_socket = _dbus_lookup_launchd_socket (&socket_path, "DBUS_LAUNCHD_SESSION_BUS_SOCKET", error);

  if (dbus_error_is_set(error))
    {
      _dbus_string_free(&socket_path);
      return FALSE;
    }

  if (!valid_socket)
    {
      dbus_set_error(error, "no socket path",
                "launchd did not provide a socket path, "
                "verify that org.freedesktop.dbus-session.plist is loaded!");
      _dbus_string_free(&socket_path);
      return FALSE;
    }
  if (!_dbus_string_append (address, "unix:path="))
    {
      _DBUS_SET_OOM (error);
      _dbus_string_free(&socket_path);
      return FALSE;
    }
  if (!_dbus_string_copy (&socket_path, 0, address,
                          _dbus_string_get_length (address)))
    {
      _DBUS_SET_OOM (error);
      _dbus_string_free(&socket_path);
      return FALSE;
    }

  _dbus_string_free(&socket_path);
  return TRUE;
}