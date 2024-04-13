_dbus_lookup_launchd_socket (DBusString *socket_path,
                             const char *launchd_env_var,
                             DBusError  *error)
{
#ifdef DBUS_ENABLE_LAUNCHD
  char *argv[4];
  int i;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  if (_dbus_check_setuid ())
    {
      dbus_set_error_const (error, DBUS_ERROR_NOT_SUPPORTED,
                            "Unable to find launchd socket when setuid");
      return FALSE;
    }

  i = 0;
  argv[i] = "launchctl";
  ++i;
  argv[i] = "getenv";
  ++i;
  argv[i] = (char*)launchd_env_var;
  ++i;
  argv[i] = NULL;
  ++i;

  _dbus_assert (i == _DBUS_N_ELEMENTS (argv));

  if (!_read_subprocess_line_argv(argv[0], TRUE, argv, socket_path, error))
    {
      return FALSE;
    }

  /* no error, but no result either */
  if (_dbus_string_get_length(socket_path) == 0)
    {
      return FALSE;
    }

  /* strip the carriage-return */
  _dbus_string_shorten(socket_path, 1);
  return TRUE;
#else /* DBUS_ENABLE_LAUNCHD */
  dbus_set_error(error, DBUS_ERROR_NOT_SUPPORTED,
                "can't lookup socket from launchd; launchd support not compiled in");
  return FALSE;
#endif
}