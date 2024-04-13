_dbus_get_tmpdir(void)
{
  /* Protected by _DBUS_LOCK_sysdeps */
  static const char* tmpdir = NULL;

  if (!_DBUS_LOCK (sysdeps))
    return NULL;

  if (tmpdir == NULL)
    {
      /* TMPDIR is what glibc uses, then
       * glibc falls back to the P_tmpdir macro which
       * just expands to "/tmp"
       */
      if (tmpdir == NULL)
        tmpdir = getenv("TMPDIR");

      /* These two env variables are probably
       * broken, but maybe some OS uses them?
       */
      if (tmpdir == NULL)
        tmpdir = getenv("TMP");
      if (tmpdir == NULL)
        tmpdir = getenv("TEMP");

      /* And this is the sane fallback. */
      if (tmpdir == NULL)
        tmpdir = "/tmp";
    }

  _DBUS_UNLOCK (sysdeps);

  _dbus_assert(tmpdir != NULL);

  return tmpdir;
}