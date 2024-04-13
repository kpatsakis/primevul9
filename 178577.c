static CURLcode multi_done(struct connectdata **connp,
                          CURLcode status,  /* an error if this is called
                                               after an error was detected */
                          bool premature)
{
  CURLcode result;
  struct connectdata *conn;
  struct Curl_easy *data;

  DEBUGASSERT(*connp);

  conn = *connp;
  data = conn->data;

  DEBUGF(infof(data, "multi_done\n"));

  if(data->state.done)
    /* Stop if multi_done() has already been called */
    return CURLE_OK;

  Curl_getoff_all_pipelines(data, conn);

  /* Cleanup possible redirect junk */
  free(data->req.newurl);
  data->req.newurl = NULL;
  free(data->req.location);
  data->req.location = NULL;

  switch(status) {
  case CURLE_ABORTED_BY_CALLBACK:
  case CURLE_READ_ERROR:
  case CURLE_WRITE_ERROR:
    /* When we're aborted due to a callback return code it basically have to
       be counted as premature as there is trouble ahead if we don't. We have
       many callbacks and protocols work differently, we could potentially do
       this more fine-grained in the future. */
    premature = TRUE;
  default:
    break;
  }

  /* this calls the protocol-specific function pointer previously set */
  if(conn->handler->done)
    result = conn->handler->done(conn, status, premature);
  else
    result = status;

  if(CURLE_ABORTED_BY_CALLBACK != result) {
    /* avoid this if we already aborted by callback to avoid this calling
       another callback */
    CURLcode rc = Curl_pgrsDone(conn);
    if(!result && rc)
      result = CURLE_ABORTED_BY_CALLBACK;
  }

  if((!premature &&
      conn->send_pipe->size + conn->recv_pipe->size != 0 &&
      !data->set.reuse_forbid &&
      !conn->bits.close)) {
    /* Stop if pipeline is not empty and we do not have to close
       connection. */
    DEBUGF(infof(data, "Connection still in use, no more multi_done now!\n"));
    return CURLE_OK;
  }

  data->state.done = TRUE; /* called just now! */
  Curl_resolver_cancel(conn);

  if(conn->dns_entry) {
    Curl_resolv_unlock(data, conn->dns_entry); /* done with this */
    conn->dns_entry = NULL;
  }

  /* if the transfer was completed in a paused state there can be buffered
     data left to write and then kill */
  free(data->state.tempwrite);
  data->state.tempwrite = NULL;

  /* if data->set.reuse_forbid is TRUE, it means the libcurl client has
     forced us to close this connection. This is ignored for requests taking
     place in a NTLM authentication handshake

     if conn->bits.close is TRUE, it means that the connection should be
     closed in spite of all our efforts to be nice, due to protocol
     restrictions in our or the server's end

     if premature is TRUE, it means this connection was said to be DONE before
     the entire request operation is complete and thus we can't know in what
     state it is for re-using, so we're forced to close it. In a perfect world
     we can add code that keep track of if we really must close it here or not,
     but currently we have no such detail knowledge.
  */

  if((data->set.reuse_forbid
#if defined(USE_NTLM)
      && !(conn->ntlm.state == NTLMSTATE_TYPE2 ||
           conn->proxyntlm.state == NTLMSTATE_TYPE2)
#endif
     ) || conn->bits.close || premature) {
    CURLcode res2 = Curl_disconnect(conn, premature); /* close connection */

    /* If we had an error already, make sure we return that one. But
       if we got a new error, return that. */
    if(!result && res2)
      result = res2;
  }
  else {
    /* the connection is no longer in use */
    if(ConnectionDone(data, conn)) {
      /* remember the most recently used connection */
      data->state.lastconnect = conn;

      infof(data, "Connection #%ld to host %s left intact\n",
            conn->connection_id,
            conn->bits.httpproxy?conn->proxy.dispname:conn->host.dispname);
    }
    else
      data->state.lastconnect = NULL;
  }

  *connp = NULL; /* to make the caller of this function better detect that
                    this was either closed or handed over to the connection
                    cache here, and therefore cannot be used from this point on
                 */
  Curl_free_request_state(data);

  return result;
}