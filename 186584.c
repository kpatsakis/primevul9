_dbus_user_info_fill (DBusUserInfo     *info,
                      const DBusString *username,
                      DBusError        *error)
{
  return fill_user_info (info, DBUS_UID_UNSET,
                         username, error);
}