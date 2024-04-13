CURLcode Curl_add_buffer_send(Curl_send_buffer *in,
                              struct connectdata *conn,

                               /* add the number of sent bytes to this
                                  counter */
                              long *bytes_written,

                               /* how much of the buffer contains body data */
                              size_t included_body_bytes,
                              int socketindex)

{
  ssize_t amount;
  CURLcode result;
  char *ptr;
  size_t size;
  struct HTTP *http = conn->data->req.protop;
  size_t sendsize;
  curl_socket_t sockfd;
  size_t headersize;

  DEBUGASSERT(socketindex <= SECONDARYSOCKET);

  sockfd = conn->sock[socketindex];

  /* The looping below is required since we use non-blocking sockets, but due
     to the circumstances we will just loop and try again and again etc */

  ptr = in->buffer;
  size = in->size_used;

  headersize = size - included_body_bytes; /* the initial part that isn't body
                                              is header */

  DEBUGASSERT(size > included_body_bytes);

  result = Curl_convert_to_network(conn->data, ptr, headersize);
  /* Curl_convert_to_network calls failf if unsuccessful */
  if(result) {
    /* conversion failed, free memory and return to the caller */
    free(in->buffer);
    free(in);
    return result;
  }


  if(conn->handler->flags & PROTOPT_SSL) {
    /* We never send more than CURL_MAX_WRITE_SIZE bytes in one single chunk
       when we speak HTTPS, as if only a fraction of it is sent now, this data
       needs to fit into the normal read-callback buffer later on and that
       buffer is using this size.
    */

    sendsize= (size > CURL_MAX_WRITE_SIZE)?CURL_MAX_WRITE_SIZE:size;

    /* OpenSSL is very picky and we must send the SAME buffer pointer to the
       library when we attempt to re-send this buffer. Sending the same data
       is not enough, we must use the exact same address. For this reason, we
       must copy the data to the uploadbuffer first, since that is the buffer
       we will be using if this send is retried later.
    */
    memcpy(conn->data->state.uploadbuffer, ptr, sendsize);
    ptr = conn->data->state.uploadbuffer;
  }
  else
    sendsize = size;

  result = Curl_write(conn, sockfd, ptr, sendsize, &amount);

  if(!result) {
    /*
     * Note that we may not send the entire chunk at once, and we have a set
     * number of data bytes at the end of the big buffer (out of which we may
     * only send away a part).
     */
    /* how much of the header that was sent */
    size_t headlen = (size_t)amount>headersize?headersize:(size_t)amount;
    size_t bodylen = amount - headlen;

    if(conn->data->set.verbose) {
      /* this data _may_ contain binary stuff */
      Curl_debug(conn->data, CURLINFO_HEADER_OUT, ptr, headlen, conn);
      if(bodylen) {
        /* there was body data sent beyond the initial header part, pass that
           on to the debug callback too */
        Curl_debug(conn->data, CURLINFO_DATA_OUT,
                   ptr+headlen, bodylen, conn);
      }
    }
    if(bodylen)
      /* since we sent a piece of the body here, up the byte counter for it
         accordingly */
      http->writebytecount += bodylen;

    /* 'amount' can never be a very large value here so typecasting it so a
       signed 31 bit value should not cause problems even if ssize_t is
       64bit */
    *bytes_written += (long)amount;

    if(http) {
      if((size_t)amount != size) {
        /* The whole request could not be sent in one system call. We must
           queue it up and send it later when we get the chance. We must not
           loop here and wait until it might work again. */

        size -= amount;

        ptr = in->buffer + amount;

        /* backup the currently set pointers */
        http->backup.fread_func = conn->fread_func;
        http->backup.fread_in = conn->fread_in;
        http->backup.postdata = http->postdata;
        http->backup.postsize = http->postsize;

        /* set the new pointers for the request-sending */
        conn->fread_func = (curl_read_callback)readmoredata;
        conn->fread_in = (void *)conn;
        http->postdata = ptr;
        http->postsize = (curl_off_t)size;

        http->send_buffer = in;
        http->sending = HTTPSEND_REQUEST;

        return CURLE_OK;
      }
      http->sending = HTTPSEND_BODY;
      /* the full buffer was sent, clean up and return */
    }
    else {
      if((size_t)amount != size)
        /* We have no continue-send mechanism now, fail. This can only happen
           when this function is used from the CONNECT sending function. We
           currently (stupidly) assume that the whole request is always sent
           away in the first single chunk.

           This needs FIXing.
        */
        return CURLE_SEND_ERROR;
      else
        conn->writechannel_inuse = FALSE;
    }
  }
  free(in->buffer);
  free(in);

  return result;
}