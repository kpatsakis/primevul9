CURLcode Curl_connect(struct Curl_easy *data,
                      struct connectdata **in_connect,
                      bool *asyncp,
                      bool *protocol_done)
{
  CURLcode result;

  *asyncp = FALSE; /* assume synchronous resolves by default */

  /* call the stuff that needs to be called */
  result = create_conn(data, in_connect, asyncp);

  if(!result) {
    /* no error */
    if((*in_connect)->send_pipe.size || (*in_connect)->recv_pipe.size)
      /* pipelining */
      *protocol_done = TRUE;
    else if(!*asyncp) {
      /* DNS resolution is done: that's either because this is a reused
         connection, in which case DNS was unnecessary, or because DNS
         really did finish already (synch resolver/fast async resolve) */
      result = Curl_setup_conn(*in_connect, protocol_done);
    }
  }

  if(result == CURLE_NO_CONNECTION_AVAILABLE) {
    *in_connect = NULL;
    return result;
  }

  if(result && *in_connect) {
    /* We're not allowed to return failure with memory left allocated
       in the connectdata struct, free those here */
    Curl_disconnect(*in_connect, FALSE); /* close the connection */
    *in_connect = NULL;           /* return a NULL */
  }

  return result;
}