_dbus_socket_can_pass_unix_fd (DBusSocket fd)
{
#ifdef SCM_RIGHTS
  union {
    struct sockaddr sa;
    struct sockaddr_storage storage;
    struct sockaddr_un un;
  } sa_buf;

  socklen_t sa_len = sizeof(sa_buf);

  _DBUS_ZERO(sa_buf);

  if (getsockname(fd.fd, &sa_buf.sa, &sa_len) < 0)
    return FALSE;

  return sa_buf.sa.sa_family == AF_UNIX;

#else
  return FALSE;

#endif
}