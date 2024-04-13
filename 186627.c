_dbus_append_keyring_directory_for_credentials (DBusString      *directory,
                                                DBusCredentials *credentials)
{
  DBusString homedir;
  DBusString dotdir;
  dbus_uid_t uid;

  _dbus_assert (credentials != NULL);
  _dbus_assert (!_dbus_credentials_are_anonymous (credentials));

  if (!_dbus_string_init (&homedir))
    return FALSE;

  uid = _dbus_credentials_get_unix_uid (credentials);
  _dbus_assert (uid != DBUS_UID_UNSET);

  if (!_dbus_homedir_from_uid (uid, &homedir))
    goto failed;

#ifdef DBUS_ENABLE_EMBEDDED_TESTS
  {
    const char *override;

    override = _dbus_getenv ("DBUS_TEST_HOMEDIR");
    if (override != NULL && *override != '\0')
      {
        _dbus_string_set_length (&homedir, 0);
        if (!_dbus_string_append (&homedir, override))
          goto failed;

        _dbus_verbose ("Using fake homedir for testing: %s\n",
                       _dbus_string_get_const_data (&homedir));
      }
    else
      {
        /* Not strictly thread-safe, but if we fail at thread-safety here,
         * the worst that will happen is some extra warnings. */
        static dbus_bool_t already_warned = FALSE;
        if (!already_warned)
          {
            _dbus_warn ("Using %s for testing, set DBUS_TEST_HOMEDIR to avoid",
                _dbus_string_get_const_data (&homedir));
            already_warned = TRUE;
          }
      }
  }
#endif

  _dbus_string_init_const (&dotdir, ".dbus-keyrings");
  if (!_dbus_concat_dir_and_file (&homedir,
                                  &dotdir))
    goto failed;

  if (!_dbus_string_copy (&homedir, 0,
                          directory, _dbus_string_get_length (directory))) {
    goto failed;
  }

  _dbus_string_free (&homedir);
  return TRUE;

 failed:
  _dbus_string_free (&homedir);
  return FALSE;
}