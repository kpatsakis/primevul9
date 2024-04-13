static CURLcode setup_connection_internals(struct connectdata *conn)
{
  const struct Curl_handler * p;
  CURLcode result;
  struct Curl_easy *data = conn->data;

  /* in some case in the multi state-machine, we go back to the CONNECT state
     and then a second (or third or...) call to this function will be made
     without doing a DISCONNECT or DONE in between (since the connection is
     yet in place) and therefore this function needs to first make sure
     there's no lingering previous data allocated. */
  Curl_free_request_state(data);

  memset(&data->req, 0, sizeof(struct SingleRequest));
  data->req.maxdownload = -1;

  conn->socktype = SOCK_STREAM; /* most of them are TCP streams */

  /* Perform setup complement if some. */
  p = conn->handler;

  if(p->setup_connection) {
    result = (*p->setup_connection)(conn);

    if(result)
      return result;

    p = conn->handler;              /* May have changed. */
  }

  if(conn->port < 0)
    /* we check for -1 here since if proxy was detected already, this
       was very likely already set to the proxy port */
    conn->port = p->defport;

  return CURLE_OK;
}