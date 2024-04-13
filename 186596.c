_dbus_delete_directory (const DBusString *filename,
                        DBusError        *error)
{
  const char *filename_c;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  filename_c = _dbus_string_get_const_data (filename);

  if (rmdir (filename_c) != 0)
    {
      dbus_set_error (error, DBUS_ERROR_FAILED,
                      "Failed to remove directory %s: %s\n",
                      filename_c, _dbus_strerror (errno));
      return FALSE;
    }

  return TRUE;
}