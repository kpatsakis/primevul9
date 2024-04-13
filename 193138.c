ConnectClientToTcpAddr(unsigned int host, int port)
{
  int sock;
  struct sockaddr_in addr;
  int one = 1;

  if (!initSockets())
	  return -1;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = host;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
#ifdef WIN32
    errno=WSAGetLastError();
#endif
    rfbClientErr("ConnectToTcpAddr: socket (%s)\n",strerror(errno));
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    rfbClientErr("ConnectToTcpAddr: connect\n");
    close(sock);
    return -1;
  }

  if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
		 (char *)&one, sizeof(one)) < 0) {
    rfbClientErr("ConnectToTcpAddr: setsockopt\n");
    close(sock);
    return -1;
  }

  return sock;
}