CURLcode Curl_protocol_connect(struct connectdata *conn,
                               bool *protocol_done)
{
  CURLcode result = CURLE_OK;

  *protocol_done = FALSE;

  if(conn->bits.tcpconnect[FIRSTSOCKET] && conn->bits.protoconnstart) {
    /* We already are connected, get back. This may happen when the connect
       worked fine in the first call, like when we connect to a local server
       or proxy. Note that we don't know if the protocol is actually done.

       Unless this protocol doesn't have any protocol-connect callback, as
       then we know we're done. */
    if(!conn->handler->connecting)
      *protocol_done = TRUE;

    return CURLE_OK;
  }

  if(!conn->bits.protoconnstart) {

    result = Curl_proxy_connect(conn, FIRSTSOCKET);
    if(result)
      return result;

    if(CONNECT_FIRSTSOCKET_PROXY_SSL())
      /* wait for HTTPS proxy SSL initialization to complete */
      return CURLE_OK;

    if(conn->bits.tunnel_proxy && conn->bits.httpproxy &&
       Curl_connect_ongoing(conn))
      /* when using an HTTP tunnel proxy, await complete tunnel establishment
         before proceeding further. Return CURLE_OK so we'll be called again */
      return CURLE_OK;

    if(conn->handler->connect_it) {
      /* is there a protocol-specific connect() procedure? */

      /* Call the protocol-specific connect function */
      result = conn->handler->connect_it(conn, protocol_done);
    }
    else
      *protocol_done = TRUE;

    /* it has started, possibly even completed but that knowledge isn't stored
       in this bit! */
    if(!result)
      conn->bits.protoconnstart = TRUE;
  }

  return result; /* pass back status */
}