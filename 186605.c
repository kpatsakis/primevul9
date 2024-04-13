_dbus_credentials_add_from_current_process (DBusCredentials *credentials)
{
  /* The POSIX spec certainly doesn't promise this, but
   * we need these assertions to fail as soon as we're wrong about
   * it so we can do the porting fixups
   */
  _DBUS_STATIC_ASSERT (sizeof (pid_t) <= sizeof (dbus_pid_t));
  _DBUS_STATIC_ASSERT (sizeof (uid_t) <= sizeof (dbus_uid_t));
  _DBUS_STATIC_ASSERT (sizeof (gid_t) <= sizeof (dbus_gid_t));

  if (!_dbus_credentials_add_pid(credentials, _dbus_getpid()))
    return FALSE;
  if (!_dbus_credentials_add_unix_uid(credentials, _dbus_geteuid()))
    return FALSE;

  return TRUE;
}