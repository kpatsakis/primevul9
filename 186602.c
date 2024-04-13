_dbus_read_local_machine_uuid (DBusGUID   *machine_id,
                               dbus_bool_t create_if_not_found,
                               DBusError  *error)
{
  DBusError our_error = DBUS_ERROR_INIT;
  DBusError etc_error = DBUS_ERROR_INIT;
  DBusString filename;
  dbus_bool_t b;

  _dbus_string_init_const (&filename, DBUS_MACHINE_UUID_FILE);

  b = _dbus_read_uuid_file (&filename, machine_id, FALSE, &our_error);
  if (b)
    return TRUE;

  /* Fallback to the system machine ID */
  _dbus_string_init_const (&filename, "/etc/machine-id");
  b = _dbus_read_uuid_file (&filename, machine_id, FALSE, &etc_error);

  if (b)
    {
      if (create_if_not_found)
        {
          /* try to copy it to the DBUS_MACHINE_UUID_FILE, but do not
           * complain if that isn't possible for whatever reason */
          _dbus_string_init_const (&filename, DBUS_MACHINE_UUID_FILE);
          _dbus_write_uuid_file (&filename, machine_id, NULL);
        }

      dbus_error_free (&our_error);
      return TRUE;
    }

  if (!create_if_not_found)
    {
      dbus_set_error (error, etc_error.name,
                      "D-Bus library appears to be incorrectly set up: "
                      "see the manual page for dbus-uuidgen to correct "
                      "this issue. (%s; %s)",
                      our_error.message, etc_error.message);
      dbus_error_free (&our_error);
      dbus_error_free (&etc_error);
      return FALSE;
    }

  dbus_error_free (&our_error);
  dbus_error_free (&etc_error);

  /* if none found, try to make a new one */
  _dbus_string_init_const (&filename, DBUS_MACHINE_UUID_FILE);

  if (!_dbus_generate_uuid (machine_id, error))
    return FALSE;

  return _dbus_write_uuid_file (&filename, machine_id, error);
}