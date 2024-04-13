_dbus_ensure_directory (const DBusString *filename,
                        DBusError        *error)
{
  const char *filename_c;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  filename_c = _dbus_string_get_const_data (filename);

  if (mkdir (filename_c, 0700) < 0)
    {
      if (errno == EEXIST)
        return TRUE;

      dbus_set_error (error, DBUS_ERROR_FAILED,
                      "Failed to create directory %s: %s\n",
                      filename_c, _dbus_strerror (errno));
      return FALSE;
    }
  else
    return TRUE;
}