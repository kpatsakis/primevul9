mbed_connect_common(struct connectdata *conn,
                    int sockindex,
                    bool nonblocking,
                    bool *done)
{
  CURLcode retcode;
  struct SessionHandle *data = conn->data;
  struct ssl_connect_data *connssl = &conn->ssl[sockindex];
  curl_socket_t sockfd = conn->sock[sockindex];
  long timeout_ms;
  int what;

  /* check if the connection has already been established */
  if(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    return CURLE_OK;
  }

  if(ssl_connect_1==connssl->connecting_state) {
    /* Find out how much more time we're allowed */
    timeout_ms = Curl_timeleft(data, NULL, TRUE);

    if(timeout_ms < 0) {
      /* no need to continue if time already is up */
      failf(data, "SSL connection timeout");
      return CURLE_OPERATION_TIMEDOUT;
    }
    retcode = mbed_connect_step1(conn, sockindex);
    if(retcode)
      return retcode;
  }

  while(ssl_connect_2 == connssl->connecting_state ||
        ssl_connect_2_reading == connssl->connecting_state ||
        ssl_connect_2_writing == connssl->connecting_state) {

    /* check allowed time left */
    timeout_ms = Curl_timeleft(data, NULL, TRUE);

    if(timeout_ms < 0) {
      /* no need to continue if time already is up */
      failf(data, "SSL connection timeout");
      return CURLE_OPERATION_TIMEDOUT;
    }

    /* if ssl is expecting something, check if it's available. */
    if(connssl->connecting_state == ssl_connect_2_reading
       || connssl->connecting_state == ssl_connect_2_writing) {

      curl_socket_t writefd = ssl_connect_2_writing==
        connssl->connecting_state?sockfd:CURL_SOCKET_BAD;
      curl_socket_t readfd = ssl_connect_2_reading==
        connssl->connecting_state?sockfd:CURL_SOCKET_BAD;

      what = Curl_socket_ready(readfd, writefd, nonblocking ? 0 : timeout_ms);
      if(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        return CURLE_SSL_CONNECT_ERROR;
      }
      else if(0 == what) {
        if(nonblocking) {
          *done = FALSE;
          return CURLE_OK;
        }
        else {
          /* timeout */
          failf(data, "SSL connection timeout");
          return CURLE_OPERATION_TIMEDOUT;
        }
      }
      /* socket is readable or writable */
    }

    /* Run transaction, and return to the caller if it failed or if
     * this connection is part of a multi handle and this loop would
     * execute again. This permits the owner of a multi handle to
     * abort a connection attempt before step2 has completed while
     * ensuring that a client using select() or epoll() will always
     * have a valid fdset to wait on.
     */
    retcode = mbed_connect_step2(conn, sockindex);
    if(retcode || (nonblocking &&
                   (ssl_connect_2 == connssl->connecting_state ||
                    ssl_connect_2_reading == connssl->connecting_state ||
                    ssl_connect_2_writing == connssl->connecting_state)))
      return retcode;

  } /* repeat step2 until all transactions are done. */

  if(ssl_connect_3==connssl->connecting_state) {
    retcode = mbed_connect_step3(conn, sockindex);
    if(retcode)
      return retcode;
  }

  if(ssl_connect_done==connssl->connecting_state) {
    connssl->state = ssl_connection_complete;
    conn->recv[sockindex] = mbed_recv;
    conn->send[sockindex] = mbed_send;
    *done = TRUE;
  }
  else
    *done = FALSE;

  /* Reset our connect state machine */
  connssl->connecting_state = ssl_connect_1;

  return CURLE_OK;
}