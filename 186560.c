fill_user_info_from_passwd (struct passwd *p,
                            DBusUserInfo  *info,
                            DBusError     *error)
{
  _dbus_assert (p->pw_name != NULL);
  _dbus_assert (p->pw_dir != NULL);

  info->uid = p->pw_uid;
  info->primary_gid = p->pw_gid;
  info->username = _dbus_strdup (p->pw_name);
  info->homedir = _dbus_strdup (p->pw_dir);

  if (info->username == NULL ||
      info->homedir == NULL)
    {
      dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
      return FALSE;
    }

  return TRUE;
}