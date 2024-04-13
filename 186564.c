_dbus_open_socket (int              *fd_p,
                   int               domain,
                   int               type,
                   int               protocol,
                   DBusError        *error)
{
#ifdef SOCK_CLOEXEC
  dbus_bool_t cloexec_done;

  *fd_p = socket (domain, type | SOCK_CLOEXEC, protocol);
  cloexec_done = *fd_p >= 0;

  /* Check if kernel seems to be too old to know SOCK_CLOEXEC */
  if (*fd_p < 0 && (errno == EINVAL || errno == EPROTOTYPE))
#endif
    {
      *fd_p = socket (domain, type, protocol);
    }

  if (*fd_p >= 0)
    {
#ifdef SOCK_CLOEXEC
      if (!cloexec_done)
#endif
        {
          _dbus_fd_set_close_on_exec(*fd_p);
        }

      _dbus_verbose ("socket fd %d opened\n", *fd_p);
      return TRUE;
    }
  else
    {
      dbus_set_error(error,
                     _dbus_error_from_errno (errno),
                     "Failed to open socket: %s",
                     _dbus_strerror (errno));
      return FALSE;
    }
}