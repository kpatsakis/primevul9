add_groups_to_credentials (int              client_fd,
                           DBusCredentials *credentials,
                           dbus_gid_t       primary)
{
#if defined(__linux__) && defined(SO_PEERGROUPS)
  _DBUS_STATIC_ASSERT (sizeof (gid_t) <= sizeof (dbus_gid_t));
  gid_t *buf = NULL;
  socklen_t len = 1024;
  dbus_bool_t oom = FALSE;
  /* libdbus has a different representation of group IDs just to annoy you */
  dbus_gid_t *converted_gids = NULL;
  dbus_bool_t need_primary = TRUE;
  size_t n_gids;
  size_t i;

  n_gids = ((size_t) len) / sizeof (gid_t);
  buf = dbus_new (gid_t, n_gids);

  if (buf == NULL)
    return FALSE;

  while (getsockopt (client_fd, SOL_SOCKET, SO_PEERGROUPS, buf, &len) < 0)
    {
      int e = errno;
      gid_t *replacement;

      _dbus_verbose ("getsockopt failed with %s, len now %lu\n",
                     _dbus_strerror (e), (unsigned long) len);

      if (e != ERANGE || (size_t) len <= n_gids * sizeof (gid_t))
        {
          _dbus_verbose ("Failed to getsockopt(SO_PEERGROUPS): %s\n",
                         _dbus_strerror (e));
          goto out;
        }

      /* If not enough space, len is updated to be enough.
       * Try again with a large enough buffer. */
      n_gids = ((size_t) len) / sizeof (gid_t);
      replacement = dbus_realloc (buf, len);

      if (replacement == NULL)
        {
          oom = TRUE;
          goto out;
        }

      buf = replacement;
      _dbus_verbose ("will try again with %lu\n", (unsigned long) len);
    }

  if (len <= 0)
    {
      _dbus_verbose ("getsockopt(SO_PEERGROUPS) yielded <= 0 bytes: %ld\n",
                     (long) len);
      goto out;
    }

  if (len > n_gids * sizeof (gid_t))
    {
      _dbus_verbose ("%lu > %zu", (unsigned long) len, n_gids * sizeof (gid_t));
      _dbus_assert_not_reached ("getsockopt(SO_PEERGROUPS) overflowed");
    }

  if (len % sizeof (gid_t) != 0)
    {
      _dbus_verbose ("getsockopt(SO_PEERGROUPS) did not return an "
                     "integer multiple of sizeof(gid_t): %lu should be "
                     "divisible by %zu",
                     (unsigned long) len, sizeof (gid_t));
      goto out;
    }

  /* Allocate an extra space for the primary group ID */
  n_gids = ((size_t) len) / sizeof (gid_t);

  /* If n_gids is less than this, then (n_gids + 1) certainly doesn't
   * overflow, and neither does multiplying that by sizeof(dbus_gid_t).
   * This is using _DBUS_INT32_MAX as a conservative lower bound for
   * the maximum size_t. */
  if (n_gids >= (_DBUS_INT32_MAX / sizeof (dbus_gid_t)) - 1)
    {
      _dbus_verbose ("getsockopt(SO_PEERGROUPS) returned a huge number "
                     "of groups (%lu bytes), ignoring",
                     (unsigned long) len);
      goto out;
    }

  converted_gids = dbus_new (dbus_gid_t, n_gids + 1);

  if (converted_gids == NULL)
    {
      oom = TRUE;
      goto out;
    }

  for (i = 0; i < n_gids; i++)
    {
      converted_gids[i] = (dbus_gid_t) buf[i];

      if (converted_gids[i] == primary)
        need_primary = FALSE;
    }

  if (need_primary && primary != DBUS_GID_UNSET)
    {
      converted_gids[n_gids] = primary;
      n_gids++;
    }

  _dbus_credentials_take_unix_gids (credentials, converted_gids, n_gids);

out:
  dbus_free (buf);
  return !oom;
#else
  /* no error */
  return TRUE;
#endif
}