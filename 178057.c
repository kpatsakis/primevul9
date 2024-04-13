static CURLcode parse_connect_to_slist(struct Curl_easy *data,
                                       struct connectdata *conn,
                                       struct curl_slist *conn_to_host)
{
  CURLcode result = CURLE_OK;
  char *host = NULL;
  int port = -1;

  while(conn_to_host && !host && port == -1) {
    result = parse_connect_to_string(data, conn, conn_to_host->data,
                                     &host, &port);
    if(result)
      return result;

    if(host && *host) {
      conn->conn_to_host.rawalloc = host;
      conn->conn_to_host.name = host;
      conn->bits.conn_to_host = TRUE;

      infof(data, "Connecting to hostname: %s\n", host);
    }
    else {
      /* no "connect to host" */
      conn->bits.conn_to_host = FALSE;
      Curl_safefree(host);
    }

    if(port >= 0) {
      conn->conn_to_port = port;
      conn->bits.conn_to_port = TRUE;
      infof(data, "Connecting to port: %d\n", port);
    }
    else {
      /* no "connect to port" */
      conn->bits.conn_to_port = FALSE;
      port = -1;
    }

    conn_to_host = conn_to_host->next;
  }

  return result;
}