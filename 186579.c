_dbus_lookup_user_bus (dbus_bool_t *supported,
                       DBusString  *address,
                       DBusError   *error)
{
  const char *runtime_dir = _dbus_getenv ("XDG_RUNTIME_DIR");
  dbus_bool_t ret = FALSE;
  struct stat stbuf;
  DBusString user_bus_path;

  if (runtime_dir == NULL)
    {
      _dbus_verbose ("XDG_RUNTIME_DIR not found in environment");
      *supported = FALSE;
      return TRUE;        /* Cannot use it, but not an error */
    }

  if (!_dbus_string_init (&user_bus_path))
    {
      _DBUS_SET_OOM (error);
      return FALSE;
    }

  if (!_dbus_string_append_printf (&user_bus_path, "%s/bus", runtime_dir))
    {
      _DBUS_SET_OOM (error);
      goto out;
    }

  if (lstat (_dbus_string_get_const_data (&user_bus_path), &stbuf) == -1)
    {
      _dbus_verbose ("XDG_RUNTIME_DIR/bus not available: %s",
                     _dbus_strerror (errno));
      *supported = FALSE;
      ret = TRUE;       /* Cannot use it, but not an error */
      goto out;
    }

  if (stbuf.st_uid != getuid ())
    {
      _dbus_verbose ("XDG_RUNTIME_DIR/bus owned by uid %ld, not our uid %ld",
                     (long) stbuf.st_uid, (long) getuid ());
      *supported = FALSE;
      ret = TRUE;       /* Cannot use it, but not an error */
      goto out;
    }

  if ((stbuf.st_mode & S_IFMT) != S_IFSOCK)
    {
      _dbus_verbose ("XDG_RUNTIME_DIR/bus is not a socket: st_mode = 0o%lo",
                     (long) stbuf.st_mode);
      *supported = FALSE;
      ret = TRUE;       /* Cannot use it, but not an error */
      goto out;
    }

  if (!_dbus_string_append (address, "unix:path=") ||
      !_dbus_address_append_escaped (address, &user_bus_path))
    {
      _DBUS_SET_OOM (error);
      goto out;
    }

  *supported = TRUE;
  ret = TRUE;

out:
  _dbus_string_free (&user_bus_path);
  return ret;
}