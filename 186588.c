_dbus_check_setuid (void)
{
  /* TODO: get __libc_enable_secure exported from glibc.
   * See http://www.openwall.com/lists/owl-dev/2012/08/14/1
   */
#if 0 && defined(HAVE_LIBC_ENABLE_SECURE)
  {
    /* See glibc/include/unistd.h */
    extern int __libc_enable_secure;
    return __libc_enable_secure;
  }
#elif defined(HAVE_ISSETUGID)
  /* BSD: http://www.freebsd.org/cgi/man.cgi?query=issetugid&sektion=2 */
  return issetugid ();
#else
  uid_t ruid, euid, suid; /* Real, effective and saved user ID's */
  gid_t rgid, egid, sgid; /* Real, effective and saved group ID's */

  /* We call into this function from _dbus_threads_init_platform_specific()
   * to make sure these are initialized before we start threading. */
  static dbus_bool_t check_setuid_initialised;
  static dbus_bool_t is_setuid;

  if (_DBUS_UNLIKELY (!check_setuid_initialised))
    {
#ifdef HAVE_GETRESUID
      if (getresuid (&ruid, &euid, &suid) != 0 ||
          getresgid (&rgid, &egid, &sgid) != 0)
#endif /* HAVE_GETRESUID */
        {
          suid = ruid = getuid ();
          sgid = rgid = getgid ();
          euid = geteuid ();
          egid = getegid ();
        }

      check_setuid_initialised = TRUE;
      is_setuid = (ruid != euid || ruid != suid ||
                   rgid != egid || rgid != sgid);

    }
  return is_setuid;
#endif
}