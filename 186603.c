_dbus_append_user_from_current_process (DBusString *str)
{
  return _dbus_string_append_uint (str,
                                   _dbus_geteuid ());
}