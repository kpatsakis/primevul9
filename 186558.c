_dbus_accept  (DBusSocket listen_fd)
{
  DBusSocket client_fd;
  struct sockaddr addr;
  socklen_t addrlen;
#ifdef HAVE_ACCEPT4
  dbus_bool_t cloexec_done;
#endif

  addrlen = sizeof (addr);

 retry:

#ifdef HAVE_ACCEPT4
  /*
   * At compile-time, we assume that if accept4() is available in
   * libc headers, SOCK_CLOEXEC is too. At runtime, it is still
   * not necessarily true that either is supported by the running kernel.
   */
  client_fd.fd = accept4 (listen_fd.fd, &addr, &addrlen, SOCK_CLOEXEC);
  cloexec_done = client_fd.fd >= 0;

  if (client_fd.fd < 0 && (errno == ENOSYS || errno == EINVAL))
#endif
    {
      client_fd.fd = accept (listen_fd.fd, &addr, &addrlen);
    }

  if (client_fd.fd < 0)
    {
      if (errno == EINTR)
        goto retry;
    }

  _dbus_verbose ("client fd %d accepted\n", client_fd.fd);

#ifdef HAVE_ACCEPT4
  if (!cloexec_done)
#endif
    {
      _dbus_fd_set_close_on_exec(client_fd.fd);
    }

  return client_fd;
}