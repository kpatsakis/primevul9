_dbus_listen_tcp_socket (const char     *host,
                         const char     *port,
                         const char     *family,
                         DBusString     *retport,
                         const char    **retfamily,
                         DBusSocket    **fds_p,
                         DBusError      *error)
{
  int saved_errno;
  int nlisten_fd = 0, res, i;
  DBusList *bind_errors = NULL;
  DBusError *bind_error = NULL;
  DBusSocket *listen_fd = NULL;
  struct addrinfo hints;
  struct addrinfo *ai, *tmp;
  unsigned int reuseaddr;
  dbus_bool_t have_ipv4 = FALSE;
  dbus_bool_t have_ipv6 = FALSE;

  *fds_p = NULL;
  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  _DBUS_ZERO (hints);

  if (!family)
    hints.ai_family = AF_UNSPEC;
  else if (!strcmp(family, "ipv4"))
    hints.ai_family = AF_INET;
  else if (!strcmp(family, "ipv6"))
    hints.ai_family = AF_INET6;
  else
    {
      dbus_set_error (error,
                      DBUS_ERROR_BAD_ADDRESS,
                      "Unknown address family %s", family);
      return -1;
    }

  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE;

 redo_lookup_with_port:
  ai = NULL;
  if ((res = getaddrinfo(host, port, &hints, &ai)) != 0 || !ai)
    {
      dbus_set_error (error,
                      _dbus_error_from_gai (res, errno),
                      "Failed to lookup host/port: \"%s:%s\": %s (%d)",
                      host ? host : "*", port, gai_strerror(res), res);
      goto failed;
    }

  tmp = ai;
  while (tmp)
    {
      int fd = -1, tcp_nodelay_on;
      DBusSocket *newlisten_fd;

      if (!_dbus_open_socket (&fd, tmp->ai_family, SOCK_STREAM, 0, error))
        {
          _DBUS_ASSERT_ERROR_IS_SET(error);
          goto failed;
        }
      _DBUS_ASSERT_ERROR_IS_CLEAR(error);

      reuseaddr = 1;
      if (setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr))==-1)
        {
          _dbus_warn ("Failed to set socket option \"%s:%s\": %s",
                      host ? host : "*", port, _dbus_strerror (errno));
        }

      /* Nagle's algorithm imposes a huge delay on the initial messages
         going over TCP. */
      tcp_nodelay_on = 1;
      if (setsockopt (fd, IPPROTO_TCP, TCP_NODELAY, &tcp_nodelay_on, sizeof (tcp_nodelay_on)) == -1)
        {
          _dbus_warn ("Failed to set TCP_NODELAY socket option \"%s:%s\": %s",
                      host ? host : "*", port, _dbus_strerror (errno));
        }

      if (bind (fd, (struct sockaddr*) tmp->ai_addr, tmp->ai_addrlen) < 0)
        {
          saved_errno = errno;
          _dbus_close(fd, NULL);

          /*
           * We don't treat this as a fatal error, because there might be
           * other addresses that we can listen on. In particular:
           *
           * - If saved_errno is EADDRINUSE after we
           *   "goto redo_lookup_with_port" after binding a port on one of the
           *   possible addresses, we will try to bind that same port on
           *   every address, including the same address again for a second
           *   time, which will fail with EADDRINUSE.
           *
           * - If saved_errno is EADDRINUSE, it might be because binding to
           *   an IPv6 address implicitly binds to a corresponding IPv4
           *   address or vice versa (e.g. Linux with bindv6only=0).
           *
           * - If saved_errno is EADDRNOTAVAIL when we asked for family
           *   AF_UNSPEC, it might be because IPv6 is disabled for this
           *   particular interface (e.g. Linux with
           *   /proc/sys/net/ipv6/conf/lo/disable_ipv6).
           */
          bind_error = dbus_new0 (DBusError, 1);

          if (bind_error == NULL)
            {
              _DBUS_SET_OOM (error);
              goto failed;
            }

          dbus_error_init (bind_error);
          _dbus_set_error_with_inet_sockaddr (bind_error, tmp->ai_addr, tmp->ai_addrlen,
                                              "Failed to bind socket",
                                              saved_errno);

          if (!_dbus_list_append (&bind_errors, bind_error))
            {
              dbus_error_free (bind_error);
              dbus_free (bind_error);
              _DBUS_SET_OOM (error);
              goto failed;
            }

          /* Try the next address, maybe it will work better */
          tmp = tmp->ai_next;
          continue;
        }

      if (listen (fd, 30 /* backlog */) < 0)
        {
          saved_errno = errno;
          _dbus_close (fd, NULL);
          _dbus_set_error_with_inet_sockaddr (error, tmp->ai_addr, tmp->ai_addrlen,
                                              "Failed to listen on socket",
                                              saved_errno);
          goto failed;
        }

      newlisten_fd = dbus_realloc(listen_fd, sizeof(DBusSocket)*(nlisten_fd+1));
      if (!newlisten_fd)
        {
          _dbus_close (fd, NULL);
          dbus_set_error (error, DBUS_ERROR_NO_MEMORY,
                          "Failed to allocate file handle array");
          goto failed;
        }
      listen_fd = newlisten_fd;
      listen_fd[nlisten_fd].fd = fd;
      nlisten_fd++;

      if (tmp->ai_addr->sa_family == AF_INET)
        have_ipv4 = TRUE;
      else if (tmp->ai_addr->sa_family == AF_INET6)
        have_ipv6 = TRUE;

      if (!_dbus_string_get_length(retport))
        {
          /* If the user didn't specify a port, or used 0, then
             the kernel chooses a port. After the first address
             is bound to, we need to force all remaining addresses
             to use the same port */
          if (!port || !strcmp(port, "0"))
            {
              int result;
              struct sockaddr_storage addr;
              socklen_t addrlen;
              char portbuf[50];

              addrlen = sizeof(addr);
              result = getsockname(fd, (struct sockaddr*) &addr, &addrlen);

              if (result == -1)
                {
                  saved_errno = errno;
                  dbus_set_error (error, _dbus_error_from_errno (saved_errno),
                                  "Failed to retrieve socket name for \"%s:%s\": %s",
                                  host ? host : "*", port, _dbus_strerror (saved_errno));
                  goto failed;
                }

              if ((res = getnameinfo ((struct sockaddr*)&addr, addrlen, NULL, 0,
                                      portbuf, sizeof(portbuf),
                                      NI_NUMERICHOST | NI_NUMERICSERV)) != 0)
                {
                  saved_errno = errno;
                  dbus_set_error (error, _dbus_error_from_gai (res, saved_errno),
                                  "Failed to resolve port \"%s:%s\": %s (%d)",
                                  host ? host : "*", port, gai_strerror(res), res);
                  goto failed;
                }

              if (!_dbus_string_append(retport, portbuf))
                {
                  dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
                  goto failed;
                }

              /* Release current address list & redo lookup */
              port = _dbus_string_get_const_data(retport);
              freeaddrinfo(ai);
              goto redo_lookup_with_port;
            }
          else
            {
              if (!_dbus_string_append(retport, port))
                {
                    dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
                    goto failed;
                }
            }
        }

      tmp = tmp->ai_next;
    }
  freeaddrinfo(ai);
  ai = NULL;

  if (!nlisten_fd)
    {
      _dbus_combine_tcp_errors (&bind_errors, "Failed to bind", host,
                                port, error);
      goto failed;
    }

  if (have_ipv4 && !have_ipv6)
    *retfamily = "ipv4";
  else if (!have_ipv4 && have_ipv6)
    *retfamily = "ipv6";

  for (i = 0 ; i < nlisten_fd ; i++)
    {
      if (!_dbus_set_fd_nonblocking (listen_fd[i].fd, error))
        {
          goto failed;
        }
    }

  *fds_p = listen_fd;

  /* This list might be non-empty even on success, because we might be
   * ignoring EADDRINUSE or EADDRNOTAVAIL */
  while ((bind_error = _dbus_list_pop_first (&bind_errors)))
    {
      dbus_error_free (bind_error);
      dbus_free (bind_error);
    }

  return nlisten_fd;

 failed:
  if (ai)
    freeaddrinfo(ai);
  for (i = 0 ; i < nlisten_fd ; i++)
    _dbus_close(listen_fd[i].fd, NULL);

  while ((bind_error = _dbus_list_pop_first (&bind_errors)))
    {
      dbus_error_free (bind_error);
      dbus_free (bind_error);
    }

  dbus_free(listen_fd);
  return -1;
}