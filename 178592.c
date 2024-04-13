static CURLcode multi_do(struct connectdata **connp, bool *done)
{
  CURLcode result=CURLE_OK;
  struct connectdata *conn = *connp;
  struct Curl_easy *data = conn->data;

  if(conn->handler->do_it) {
    /* generic protocol-specific function pointer set in curl_connect() */
    result = conn->handler->do_it(conn, done);

    /* This was formerly done in transfer.c, but we better do it here */
    if((CURLE_SEND_ERROR == result) && conn->bits.reuse) {
      /*
       * If the connection is using an easy handle, call reconnect
       * to re-establish the connection.  Otherwise, let the multi logic
       * figure out how to re-establish the connection.
       */
      if(!data->multi) {
        result = multi_reconnect_request(connp);

        if(!result) {
          /* ... finally back to actually retry the DO phase */
          conn = *connp; /* re-assign conn since multi_reconnect_request
                            creates a new connection */
          result = conn->handler->do_it(conn, done);
        }
      }
      else
        return result;
    }

    if(!result && *done)
      /* do_complete must be called after the protocol-specific DO function */
      do_complete(conn);
  }
  return result;
}