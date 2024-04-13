_dbus_user_info_fill_uid (DBusUserInfo *info,
                          dbus_uid_t    uid,
                          DBusError    *error)
{
  return fill_user_info (info, uid,
                         NULL, error);
}