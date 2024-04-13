xauth_entry_should_propagate (Xauth *xa,
                              char  *hostname,
                              char  *number)
{
  /* ensure entry isn't for remote access */
  if (xa->family != FamilyLocal && xa->family != FamilyWild)
    return FALSE;

  /* ensure entry is for this machine */
  if (xa->family == FamilyLocal && !auth_streq (hostname, xa->address, xa->address_length))
    {
      /* OpenSUSE inherits the hostname value from DHCP without updating
       * its X11 authentication cookie. The old hostname value can still
       * be found in the environment variable XAUTHLOCALHOSTNAME.
       * For reference:
       * https://bugzilla.opensuse.org/show_bug.cgi?id=262309
       * For this reason if we have a cookie whose address is equal to the
       * variable XAUTHLOCALHOSTNAME, we still need to propagate it, but
       * we also need to change its address to `unames.nodename`.
       */
      const char *xauth_local_hostname;
      xauth_local_hostname = g_getenv ("XAUTHLOCALHOSTNAME");
      if (xauth_local_hostname == NULL)
        return FALSE;

      if (!auth_streq ((char *) xauth_local_hostname, xa->address, xa->address_length))
        return FALSE;
    }

  /* ensure entry is for this session */
  if (xa->number != NULL && !auth_streq (number, xa->number, xa->number_length))
    return FALSE;

  return TRUE;
}