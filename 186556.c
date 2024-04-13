_dbus_connect_tcp_socket_with_nonce (const char     *host,
                                     const char     *port,
                                     const char     *family,
                                     const char     *noncefile,
                                     DBusError      *error)
{
  int saved_errno = 0;
  DBusList *connect_errors = NULL;
  DBusSocket fd = DBUS_SOCKET_INIT;
  int res;
  struct addrinfo hints;
  struct addrinfo *ai = NULL;
  const struct addrinfo *tmp;
  DBusError *connect_error;

  _DBUS_ASSERT_ERROR_IS_CLEAR(error);

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
      return _dbus_socket_get_invalid ();
    }
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_ADDRCONFIG;

  if ((res = getaddrinfo(host, port, &hints, &ai)) != 0)
    {
      dbus_set_error (error,
                      _dbus_error_from_gai (res, errno),
                      "Failed to lookup host/port: \"%s:%s\": %s (%d)",
                      host, port, gai_strerror(res), res);
      _dbus_socket_invalidate (&fd);
      goto out;
    }

  tmp = ai;
  while (tmp)
    {
      if (!_dbus_open_socket (&fd.fd, tmp->ai_family, SOCK_STREAM, 0, error))
        {
          _DBUS_ASSERT_ERROR_IS_SET(error);
          _dbus_socket_invalidate (&fd);
          goto out;
        }
      _DBUS_ASSERT_ERROR_IS_CLEAR(error);

      if (connect (fd.fd, (struct sockaddr*) tmp->ai_addr, tmp->ai_addrlen) < 0)
        {
          saved_errno = errno;
          _dbus_close (fd.fd, NULL);
          _dbus_socket_invalidate (&fd);

          connect_error = dbus_new0 (DBusError, 1);

          if (connect_error == NULL)
            {
              _DBUS_SET_OOM (error);
              goto out;
            }

          dbus_error_init (connect_error);
          _dbus_set_error_with_inet_sockaddr (connect_error,
                                              tmp->ai_addr, tmp->ai_addrlen,
                                              "Failed to connect to socket",
                                              saved_errno);

          if (!_dbus_list_append (&connect_errors, connect_error))
            {
              dbus_error_free (connect_error);
              dbus_free (connect_error);
              _DBUS_SET_OOM (error);
              goto out;
            }

          tmp = tmp->ai_next;
          continue;
        }

      break;
    }

  if (!_dbus_socket_is_valid (fd))
    {
      _dbus_combine_tcp_errors (&connect_errors, "Failed to connect",
                                host, port, error);
      goto out;
    }

  if (noncefile != NULL)
    {
      DBusString noncefileStr;
      dbus_bool_t ret;
      _dbus_string_init_const (&noncefileStr, noncefile);
      ret = _dbus_send_nonce (fd, &noncefileStr, error);

      if (!ret)
        {
          _dbus_close (fd.fd, NULL);
          _dbus_socket_invalidate (&fd);
          goto out;
        }
    }

  if (!_dbus_set_fd_nonblocking (fd.fd, error))
    {
      _dbus_close (fd.fd, NULL);
      _dbus_socket_invalidate (&fd);
      goto out;
    }

out:
  if (ai != NULL)
    freeaddrinfo (ai);

  while ((connect_error = _dbus_list_pop_first (&connect_errors)))
    {
      dbus_error_free (connect_error);
      dbus_free (connect_error);
    }

  return fd;
}