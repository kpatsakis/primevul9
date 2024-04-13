_dbus_connect_tcp_socket (const char     *host,
                          const char     *port,
                          const char     *family,
                          DBusError      *error)
{
    return _dbus_connect_tcp_socket_with_nonce (host, port, family, (const char*)NULL, error);
}