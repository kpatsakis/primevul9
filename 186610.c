_dbus_get_autolaunch_address (const char *scope,
                              DBusString *address,
                              DBusError  *error)
{
#ifdef DBUS_ENABLE_X11_AUTOLAUNCH
  static const char arg_dbus_launch[] = "dbus-launch";
  static const char arg_autolaunch[] = "--autolaunch";
  static const char arg_binary_syntax[] = "--binary-syntax";
  static const char arg_close_stderr[] = "--close-stderr";

  /* Perform X11-based autolaunch. (We also support launchd-based autolaunch,
   * but that's done elsewhere, and if it worked, this function wouldn't
   * be called.) */
  const char *display;
  const char *progpath;
  const char *argv[6];
  int i;
  DBusString uuid;
  dbus_bool_t retval;

  if (_dbus_check_setuid ())
    {
      dbus_set_error_const (error, DBUS_ERROR_NOT_SUPPORTED,
                            "Unable to autolaunch when setuid");
      return FALSE;
    }

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);
  retval = FALSE;

  /* fd.o #19997: if $DISPLAY isn't set to something useful, then
   * dbus-launch-x11 is just going to fail. Rather than trying to
   * run it, we might as well bail out early with a nice error.
   *
   * This is not strictly true in a world where the user bus exists,
   * because dbus-launch --autolaunch knows how to connect to that -
   * but if we were going to connect to the user bus, we'd have done
   * so before trying autolaunch: in any case. */
  display = _dbus_getenv ("DISPLAY");

  if (display == NULL || display[0] == '\0')
    {
      dbus_set_error_const (error, DBUS_ERROR_NOT_SUPPORTED,
          "Unable to autolaunch a dbus-daemon without a $DISPLAY for X11");
      return FALSE;
    }

  if (!_dbus_string_init (&uuid))
    {
      _DBUS_SET_OOM (error);
      return FALSE;
    }

  if (!_dbus_get_local_machine_uuid_encoded (&uuid, error))
    {
      goto out;
    }

#ifdef DBUS_ENABLE_EMBEDDED_TESTS
  progpath = _dbus_getenv ("DBUS_TEST_DBUS_LAUNCH");

  if (progpath == NULL)
#endif
    progpath = DBUS_BINDIR "/dbus-launch";
  /*
   * argv[0] is always dbus-launch, that's the name what we'll
   * get from /proc, or ps(1), regardless what the progpath is,
   * see fd.o#69716
   */
  i = 0;
  argv[i] = arg_dbus_launch;
  ++i;
  argv[i] = arg_autolaunch;
  ++i;
  argv[i] = _dbus_string_get_data (&uuid);
  ++i;
  argv[i] = arg_binary_syntax;
  ++i;
  argv[i] = arg_close_stderr;
  ++i;
  argv[i] = NULL;
  ++i;

  _dbus_assert (i == _DBUS_N_ELEMENTS (argv));

  retval = _read_subprocess_line_argv (progpath,
                                       TRUE,
                                       argv, address, error);

 out:
  _dbus_string_free (&uuid);
  return retval;
#else
  dbus_set_error_const (error, DBUS_ERROR_NOT_SUPPORTED,
      "Using X11 for dbus-daemon autolaunch was disabled at compile time, "
      "set your DBUS_SESSION_BUS_ADDRESS instead");
  return FALSE;
#endif
}