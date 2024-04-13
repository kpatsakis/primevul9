ConnectionDone(struct Curl_easy *data, struct connectdata *conn)
{
  /* data->multi->maxconnects can be negative, deal with it. */
  size_t maxconnects =
    (data->multi->maxconnects < 0) ? data->multi->num_easy * 4:
    data->multi->maxconnects;
  struct connectdata *conn_candidate = NULL;

  /* Mark the current connection as 'unused' */
  conn->inuse = FALSE;

  if(maxconnects > 0 &&
     data->state.conn_cache->num_connections > maxconnects) {
    infof(data, "Connection cache is full, closing the oldest one.\n");

    conn_candidate = Curl_oldest_idle_connection(data);

    if(conn_candidate) {
      /* Set the connection's owner correctly */
      conn_candidate->data = data;

      /* the winner gets the honour of being disconnected */
      (void)Curl_disconnect(conn_candidate, /* dead_connection */ FALSE);
    }
  }

  return (conn_candidate == conn) ? FALSE : TRUE;
}