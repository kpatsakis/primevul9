CURLcode Curl_disconnect(struct connectdata *conn, bool dead_connection)
{
  struct SessionHandle *data;
  if(!conn)
    return CURLE_OK; /* this is closed and fine already */
  data = conn->data;

  if(!data) {
    DEBUGF(fprintf(stderr, "DISCONNECT without easy handle, ignoring\n"));
    return CURLE_OK;
  }

  if(conn->dns_entry != NULL) {
    Curl_resolv_unlock(data, conn->dns_entry);
    conn->dns_entry = NULL;
  }

  Curl_hostcache_prune(data); /* kill old DNS cache entries */

#if !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)
  /* Cleanup NTLM connection-related data */
  Curl_http_ntlm_cleanup(conn);
#endif

  if(conn->handler->disconnect)
    /* This is set if protocol-specific cleanups should be made */
    conn->handler->disconnect(conn, dead_connection);

    /* unlink ourselves! */
  infof(data, "Closing connection %ld\n", conn->connection_id);
  Curl_conncache_remove_conn(data->state.conn_cache, conn);

#if defined(USE_LIBIDN)
  if(conn->host.encalloc)
    idn_free(conn->host.encalloc); /* encoded host name buffer, must be freed
                                      with idn_free() since this was allocated
                                      by libidn */
  if(conn->proxy.encalloc)
    idn_free(conn->proxy.encalloc); /* encoded proxy name buffer, must be
                                       freed with idn_free() since this was
                                       allocated by libidn */
#elif defined(USE_WIN32_IDN)
  free(conn->host.encalloc); /* encoded host name buffer, must be freed with
                                idn_free() since this was allocated by
                                curl_win32_idn_to_ascii */
  if(conn->proxy.encalloc)
    free(conn->proxy.encalloc); /* encoded proxy name buffer, must be freed
                                   with idn_free() since this was allocated by
                                   curl_win32_idn_to_ascii */
#endif

  Curl_ssl_close(conn, FIRSTSOCKET);

  /* Indicate to all handles on the pipe that we're dead */
  if(Curl_multi_pipeline_enabled(data->multi)) {
    signalPipeClose(conn->send_pipe, TRUE);
    signalPipeClose(conn->recv_pipe, TRUE);
  }

  conn_free(conn);

  return CURLE_OK;
}