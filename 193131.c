ListenAtTcpPortAndAddress(int port, const char *address)
{
  int sock;
  int one = 1;
#ifndef LIBVNCSERVER_IPv6
  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (address) {
    addr.sin_addr.s_addr = inet_addr(address);
  } else {
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
  }

  if (!initSockets())
    return -1;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    rfbClientErr("ListenAtTcpPort: socket\n");
    return -1;
  }

  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
		 (const char *)&one, sizeof(one)) < 0) {
    rfbClientErr("ListenAtTcpPort: setsockopt\n");
    close(sock);
    return -1;
  }

  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    rfbClientErr("ListenAtTcpPort: bind\n");
    close(sock);
    return -1;
  }

#else
  int rv;
  struct addrinfo hints, *servinfo, *p;
  char port_str[8];

  snprintf(port_str, 8, "%d", port);

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; /* fill in wildcard address if address == NULL */

  if (!initSockets())
    return -1;

  if ((rv = getaddrinfo(address, port_str, &hints, &servinfo)) != 0) {
    rfbClientErr("ListenAtTcpPortAndAddress: error in getaddrinfo: %s\n", gai_strerror(rv));
    return -1;
  }

  /* loop through all the results and bind to the first we can */
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
      continue;
    }

#ifdef IPV6_V6ONLY
    /* we have separate IPv4 and IPv6 sockets since some OS's do not support dual binding */
    if (p->ai_family == AF_INET6 && setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&one, sizeof(one)) < 0) {
      rfbClientErr("ListenAtTcpPortAndAddress: error in setsockopt IPV6_V6ONLY: %s\n", strerror(errno));
      close(sock);
      freeaddrinfo(servinfo);
      return -1;
    }
#endif

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one)) < 0) {
      rfbClientErr("ListenAtTcpPortAndAddress: error in setsockopt SO_REUSEADDR: %s\n", strerror(errno));
      close(sock);
      freeaddrinfo(servinfo);
      return -1;
    }

    if (bind(sock, p->ai_addr, p->ai_addrlen) < 0) {
      close(sock);
      continue;
    }

    break;
  }

  if (p == NULL)  {
    rfbClientErr("ListenAtTcpPortAndAddress: error in bind: %s\n", strerror(errno));
    return -1;
  }

  /* all done with this structure now */
  freeaddrinfo(servinfo);
#endif

  if (listen(sock, 5) < 0) {
    rfbClientErr("ListenAtTcpPort: listen\n");
    close(sock);
    return -1;
  }

  return sock;
}