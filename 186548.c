_dbus_lookup_session_address (dbus_bool_t *supported,
                              DBusString  *address,
                              DBusError   *error)
{
#ifdef DBUS_ENABLE_LAUNCHD
  *supported = TRUE;
  return _dbus_lookup_session_address_launchd (address, error);
#else
  *supported = FALSE;

  if (!_dbus_lookup_user_bus (supported, address, error))
    return FALSE;
  else if (*supported)
    return TRUE;

  /* On non-Mac Unix platforms, if the session address isn't already
   * set in DBUS_SESSION_BUS_ADDRESS environment variable and the
   * $XDG_RUNTIME_DIR/bus can't be used, we punt and fall back to the
   * autolaunch: global default; see init_session_address in
   * dbus/dbus-bus.c. */
  return TRUE;
#endif
}