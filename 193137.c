WriteToRFBServer(rfbClient* client, char *buf, int n)
{
  fd_set fds;
  int i = 0;
  int j;
  const char *obuf = buf;
#ifdef LIBVNCSERVER_HAVE_SASL
  const char *output;
  unsigned int outputlen;
  int err;
#endif /* LIBVNCSERVER_HAVE_SASL */

  if (client->serverPort==-1)
    return TRUE; /* vncrec playing */

  if (client->tlsSession) {
    /* WriteToTLS() will guarantee either everything is written, or error/eof returns */
    i = WriteToTLS(client, buf, n);
    if (i <= 0) return FALSE;

    return TRUE;
  }
#ifdef LIBVNCSERVER_HAVE_SASL
  if (client->saslconn) {
    err = sasl_encode(client->saslconn,
                      buf, n,
                      &output, &outputlen);
    if (err != SASL_OK) {
      rfbClientLog("Failed to encode SASL data %s",
                   sasl_errstring(err, NULL, NULL));
      return FALSE;
    }
    obuf = output;
    n = outputlen;
  }
#endif /* LIBVNCSERVER_HAVE_SASL */

  while (i < n) {
    j = write(client->sock, obuf + i, (n - i));
    if (j <= 0) {
      if (j < 0) {
#ifdef WIN32
	 errno=WSAGetLastError();
#endif
	if (errno == EWOULDBLOCK ||
#ifdef LIBVNCSERVER_ENOENT_WORKAROUND
		errno == ENOENT ||
#endif
		errno == EAGAIN) {
	  FD_ZERO(&fds);
	  FD_SET(client->sock,&fds);

	  if (select(client->sock+1, NULL, &fds, NULL, NULL) <= 0) {
	    rfbClientErr("select\n");
	    return FALSE;
	  }
	  j = 0;
	} else {
	  rfbClientErr("write\n");
	  return FALSE;
	}
      } else {
	rfbClientLog("write failed\n");
	return FALSE;
      }
    }
    i += j;
  }
  return TRUE;
}