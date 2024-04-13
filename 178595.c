static int waitproxyconnect_getsock(struct connectdata *conn,
                                    curl_socket_t *sock,
                                    int numsocks)
{
  if(!numsocks)
    return GETSOCK_BLANK;

  sock[0] = conn->sock[FIRSTSOCKET];

  /* when we've sent a CONNECT to a proxy, we should rather wait for the
     socket to become readable to be able to get the response headers */
  if(conn->tunnel_state[FIRSTSOCKET] == TUNNEL_CONNECT)
    return GETSOCK_READSOCK(0);

  return GETSOCK_WRITESOCK(0);
}