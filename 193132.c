ConnectClientToTcpAddr6(const char *hostname, int port)
{
#ifdef LIBVNCSERVER_IPv6
  int sock;
  int n;
  struct addrinfo hints, *res, *ressave;
  char port_s[10];
  int one = 1;

  if (!initSockets())
	  return -1;

  snprintf(port_s, 10, "%d", port);
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if ((n = getaddrinfo(hostname, port_s, &hints, &res)))
  {
    rfbClientErr("ConnectClientToTcpAddr6: getaddrinfo (%s)\n", gai_strerror(n));
    return -1;
  }

  ressave = res;
  sock = -1;
  while (res)
  {
    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock >= 0)
    {
      if (connect(sock, res->ai_addr, res->ai_addrlen) == 0)
        break;
      close(sock);
      sock = -1;
    }
    res = res->ai_next;
  }
  freeaddrinfo(ressave);

  if (sock == -1)
  {
    rfbClientErr("ConnectClientToTcpAddr6: connect\n");
    return -1;
  }

  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
		 (char *)&one, sizeof(one)) < 0) {
    rfbClientErr("ConnectToTcpAddr: setsockopt\n");
    close(sock);
    return -1;
  }

  return sock;

#else

  rfbClientErr("ConnectClientToTcpAddr6: IPv6 disabled\n");
  return -1;

#endif
}