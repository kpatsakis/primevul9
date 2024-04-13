_dbus_append_address_from_socket (DBusSocket  fd,
                                  DBusString *address,
                                  DBusError  *error)
{
  union {
      struct sockaddr sa;
      struct sockaddr_storage storage;
      struct sockaddr_un un;
      struct sockaddr_in ipv4;
      struct sockaddr_in6 ipv6;
  } socket;
  char hostip[INET6_ADDRSTRLEN];
  socklen_t size = sizeof (socket);
  DBusString path_str;
  const char *family_name = NULL;
  dbus_uint16_t port;

  if (getsockname (fd.fd, &socket.sa, &size))
    goto err;

  switch (socket.sa.sa_family)
    {
    case AF_UNIX:
      if (socket.un.sun_path[0]=='\0')
        {
          _dbus_string_init_const (&path_str, &(socket.un.sun_path[1]));
          if (_dbus_string_append (address, "unix:abstract=") &&
              _dbus_address_append_escaped (address, &path_str))
            {
              return TRUE;
            }
          else
            {
              _DBUS_SET_OOM (error);
              return FALSE;
            }
        }
      else
        {
          _dbus_string_init_const (&path_str, socket.un.sun_path);
          if (_dbus_string_append (address, "unix:path=") &&
              _dbus_address_append_escaped (address, &path_str))
            {
              return TRUE;
            }
          else
            {
              _DBUS_SET_OOM (error);
              return FALSE;
            }
        }
      /* not reached */
      break;

    case AF_INET:
#ifdef AF_INET6
    case AF_INET6:
#endif
       _dbus_string_init_const (&path_str, hostip);

      if (_dbus_inet_sockaddr_to_string (&socket, size, hostip, sizeof (hostip),
                                         &family_name, &port, error))
        {
          if (_dbus_string_append_printf (address, "tcp:family=%s,port=%u,host=",
                                          family_name, port) &&
              _dbus_address_append_escaped (address, &path_str))
            {
              return TRUE;
            }
          else
            {
              _DBUS_SET_OOM (error);
              return FALSE;
            }
        }
      else
        {
          return FALSE;
        }
      /* not reached */
      break;

    default:
      dbus_set_error (error,
                      _dbus_error_from_errno (EINVAL),
                      "Failed to read address from socket: Unknown socket type.");
      return FALSE;
    }
 err:
  dbus_set_error (error,
                  _dbus_error_from_errno (errno),
                  "Failed to read address from socket: %s",
                  _dbus_strerror (errno));
  return FALSE;
}