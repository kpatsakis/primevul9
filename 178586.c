static CURLcode multi_reconnect_request(struct connectdata **connp)
{
  CURLcode result = CURLE_OK;
  struct connectdata *conn = *connp;
  struct Curl_easy *data = conn->data;

  /* This was a re-use of a connection and we got a write error in the
   * DO-phase. Then we DISCONNECT this connection and have another attempt to
   * CONNECT and then DO again! The retry cannot possibly find another
   * connection to re-use, since we only keep one possible connection for
   * each.  */

  infof(data, "Re-used connection seems dead, get a new one\n");

  connclose(conn, "Reconnect dead connection"); /* enforce close */
  result = multi_done(&conn, result, FALSE); /* we are so done with this */

  /* conn may no longer be a good pointer, clear it to avoid mistakes by
     parent functions */
  *connp = NULL;

  /*
   * We need to check for CURLE_SEND_ERROR here as well. This could happen
   * when the request failed on a FTP connection and thus multi_done() itself
   * tried to use the connection (again).
   */
  if(!result || (CURLE_SEND_ERROR == result)) {
    bool async;
    bool protocol_done = TRUE;

    /* Now, redo the connect and get a new connection */
    result = Curl_connect(data, connp, &async, &protocol_done);
    if(!result) {
      /* We have connected or sent away a name resolve query fine */

      conn = *connp; /* setup conn to again point to something nice */
      if(async) {
        /* Now, if async is TRUE here, we need to wait for the name
           to resolve */
        result = Curl_resolver_wait_resolv(conn, NULL);
        if(result)
          return result;

        /* Resolved, continue with the connection */
        result = Curl_async_resolved(conn, &protocol_done);
        if(result)
          return result;
      }
    }
  }

  return result;
}