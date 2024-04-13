CURLcode Curl_readwrite(struct connectdata *conn,
                        struct Curl_easy *data,
                        bool *done,
                        bool *comeback)
{
  struct SingleRequest *k = &data->req;
  CURLcode result;
  int didwhat = 0;

  curl_socket_t fd_read;
  curl_socket_t fd_write;
  int select_res = conn->cselect_bits;

  conn->cselect_bits = 0;

  /* only use the proper socket if the *_HOLD bit is not set simultaneously as
     then we are in rate limiting state in that transfer direction */

  if((k->keepon & KEEP_RECVBITS) == KEEP_RECV)
    fd_read = conn->sockfd;
  else
    fd_read = CURL_SOCKET_BAD;

  if((k->keepon & KEEP_SENDBITS) == KEEP_SEND)
    fd_write = conn->writesockfd;
  else
    fd_write = CURL_SOCKET_BAD;

  if(data->state.drain) {
    select_res |= CURL_CSELECT_IN;
    DEBUGF(infof(data, "Curl_readwrite: forcibly told to drain data\n"));
  }

  if(!select_res) /* Call for select()/poll() only, if read/write/error
                     status is not known. */
    select_res = Curl_socket_check(fd_read, CURL_SOCKET_BAD, fd_write, 0);

  if(select_res == CURL_CSELECT_ERR) {
    failf(data, "select/poll returned error");
    return CURLE_SEND_ERROR;
  }

#ifdef USE_HYPER
  if(conn->datastream)
    return conn->datastream(data, conn, &didwhat, done, select_res);
#endif
  /* We go ahead and do a read if we have a readable socket or if
     the stream was rewound (in which case we have data in a
     buffer) */
  if((k->keepon & KEEP_RECV) && (select_res & CURL_CSELECT_IN)) {
    result = readwrite_data(data, conn, k, &didwhat, done, comeback);
    if(result || *done)
      return result;
  }

  /* If we still have writing to do, we check if we have a writable socket. */
  if((k->keepon & KEEP_SEND) && (select_res & CURL_CSELECT_OUT)) {
    /* write */

    result = readwrite_upload(data, conn, &didwhat);
    if(result)
      return result;
  }

  k->now = Curl_now();
  if(!didwhat) {
    /* no read no write, this is a timeout? */
    if(k->exp100 == EXP100_AWAITING_CONTINUE) {
      /* This should allow some time for the header to arrive, but only a
         very short time as otherwise it'll be too much wasted time too
         often. */

      /* Quoting RFC2616, section "8.2.3 Use of the 100 (Continue) Status":

         Therefore, when a client sends this header field to an origin server
         (possibly via a proxy) from which it has never seen a 100 (Continue)
         status, the client SHOULD NOT wait for an indefinite period before
         sending the request body.

      */

      timediff_t ms = Curl_timediff(k->now, k->start100);
      if(ms >= data->set.expect_100_timeout) {
        /* we've waited long enough, continue anyway */
        k->exp100 = EXP100_SEND_DATA;
        k->keepon |= KEEP_SEND;
        Curl_expire_done(data, EXPIRE_100_TIMEOUT);
        infof(data, "Done waiting for 100-continue\n");
      }
    }
  }

  if(Curl_pgrsUpdate(data))
    result = CURLE_ABORTED_BY_CALLBACK;
  else
    result = Curl_speedcheck(data, k->now);
  if(result)
    return result;

  if(k->keepon) {
    if(0 > Curl_timeleft(data, &k->now, FALSE)) {
      if(k->size != -1) {
        failf(data, "Operation timed out after %" CURL_FORMAT_TIMEDIFF_T
              " milliseconds with %" CURL_FORMAT_CURL_OFF_T " out of %"
              CURL_FORMAT_CURL_OFF_T " bytes received",
              Curl_timediff(k->now, data->progress.t_startsingle),
              k->bytecount, k->size);
      }
      else {
        failf(data, "Operation timed out after %" CURL_FORMAT_TIMEDIFF_T
              " milliseconds with %" CURL_FORMAT_CURL_OFF_T " bytes received",
              Curl_timediff(k->now, data->progress.t_startsingle),
              k->bytecount);
      }
      return CURLE_OPERATION_TIMEDOUT;
    }
  }
  else {
    /*
     * The transfer has been performed. Just make some general checks before
     * returning.
     */

    if(!(data->set.opt_no_body) && (k->size != -1) &&
       (k->bytecount != k->size) &&
#ifdef CURL_DO_LINEEND_CONV
       /* Most FTP servers don't adjust their file SIZE response for CRLFs,
          so we'll check to see if the discrepancy can be explained
          by the number of CRLFs we've changed to LFs.
       */
       (k->bytecount != (k->size + data->state.crlf_conversions)) &&
#endif /* CURL_DO_LINEEND_CONV */
       !k->newurl) {
      failf(data, "transfer closed with %" CURL_FORMAT_CURL_OFF_T
            " bytes remaining to read", k->size - k->bytecount);
      return CURLE_PARTIAL_FILE;
    }
    if(!(data->set.opt_no_body) && k->chunk &&
       (conn->chunk.state != CHUNK_STOP)) {
      /*
       * In chunked mode, return an error if the connection is closed prior to
       * the empty (terminating) chunk is read.
       *
       * The condition above used to check for
       * conn->proto.http->chunk.datasize != 0 which is true after reading
       * *any* chunk, not just the empty chunk.
       *
       */
      failf(data, "transfer closed with outstanding read data remaining");
      return CURLE_PARTIAL_FILE;
    }
    if(Curl_pgrsUpdate(data))
      return CURLE_ABORTED_BY_CALLBACK;
  }

  /* Now update the "done" boolean we return */
  *done = (0 == (k->keepon&(KEEP_RECV|KEEP_SEND|
                            KEEP_RECV_PAUSE|KEEP_SEND_PAUSE))) ? TRUE : FALSE;

  return CURLE_OK;
}