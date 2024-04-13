static int waitconnect_getsock(struct connectdata *conn,
                               curl_socket_t *sock,
                               int numsocks)
{
  int i;
  int s=0;
  int rc=0;

  if(!numsocks)
    return GETSOCK_BLANK;

  for(i=0; i<2; i++) {
    if(conn->tempsock[i] != CURL_SOCKET_BAD) {
      sock[s] = conn->tempsock[i];
      rc |= GETSOCK_WRITESOCK(s++);
    }
  }

  return rc;
}