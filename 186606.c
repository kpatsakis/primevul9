_dbus_check_dir_is_private_to_user (DBusString *dir, DBusError *error)
{
  const char *directory;
  struct stat sb;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  directory = _dbus_string_get_const_data (dir);

  if (stat (directory, &sb) < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "%s", _dbus_strerror (errno));

      return FALSE;
    }

  if (sb.st_uid != geteuid ())
    {
      dbus_set_error (error, DBUS_ERROR_FAILED,
                     "%s directory is owned by user %lu, not %lu",
                     directory,
                     (unsigned long) sb.st_uid,
                     (unsigned long) geteuid ());
      return FALSE;
    }

  if ((S_IROTH & sb.st_mode) || (S_IWOTH & sb.st_mode) ||
      (S_IRGRP & sb.st_mode) || (S_IWGRP & sb.st_mode))
    {
      dbus_set_error (error, DBUS_ERROR_FAILED,
                     "%s directory is not private to the user", directory);
      return FALSE;
    }

  return TRUE;
}