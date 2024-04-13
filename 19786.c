static struct connectdata *allocate_conn(struct SessionHandle *data)
{
  struct connectdata *conn = calloc(1, sizeof(struct connectdata));
  if(!conn)
    return NULL;

  conn->handler = &Curl_handler_dummy;  /* Be sure we have a handler defined
                                           already from start to avoid NULL
                                           situations and checks */

  /* and we setup a few fields in case we end up actually using this struct */

  conn->sock[FIRSTSOCKET] = CURL_SOCKET_BAD;     /* no file descriptor */
  conn->sock[SECONDARYSOCKET] = CURL_SOCKET_BAD; /* no file descriptor */
  conn->tempsock[0] = CURL_SOCKET_BAD; /* no file descriptor */
  conn->tempsock[1] = CURL_SOCKET_BAD; /* no file descriptor */
  conn->connection_id = -1;    /* no ID */
  conn->port = -1; /* unknown at this point */
  conn->remote_port = -1; /* unknown */

  /* Default protocol-independent behavior doesn't support persistent
     connections, so we set this to force-close. Protocols that support
     this need to set this to FALSE in their "curl_do" functions. */
  connclose(conn, "Default to force-close");

  /* Store creation time to help future close decision making */
  conn->created = Curl_tvnow();

  conn->data = data; /* Setup the association between this connection
                        and the SessionHandle */

  conn->proxytype = data->set.proxytype; /* type */

#ifdef CURL_DISABLE_PROXY

  conn->bits.proxy = FALSE;
  conn->bits.httpproxy = FALSE;
  conn->bits.proxy_user_passwd = FALSE;
  conn->bits.tunnel_proxy = FALSE;

#else /* CURL_DISABLE_PROXY */

  /* note that these two proxy bits are now just on what looks to be
     requested, they may be altered down the road */
  conn->bits.proxy = (data->set.str[STRING_PROXY] &&
                      *data->set.str[STRING_PROXY])?TRUE:FALSE;
  conn->bits.httpproxy = (conn->bits.proxy &&
                          (conn->proxytype == CURLPROXY_HTTP ||
                           conn->proxytype == CURLPROXY_HTTP_1_0))?TRUE:FALSE;
  conn->bits.proxy_user_passwd =
    (NULL != data->set.str[STRING_PROXYUSERNAME])?TRUE:FALSE;
  conn->bits.tunnel_proxy = data->set.tunnel_thru_httpproxy;

#endif /* CURL_DISABLE_PROXY */

  conn->bits.user_passwd = (NULL != data->set.str[STRING_USERNAME])?TRUE:FALSE;
  conn->bits.ftp_use_epsv = data->set.ftp_use_epsv;
  conn->bits.ftp_use_eprt = data->set.ftp_use_eprt;

  conn->verifypeer = data->set.ssl.verifypeer;
  conn->verifyhost = data->set.ssl.verifyhost;

  conn->ip_version = data->set.ipver;

#if !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM) && \
    defined(NTLM_WB_ENABLED)
  conn->ntlm_auth_hlpr_socket = CURL_SOCKET_BAD;
  conn->ntlm_auth_hlpr_pid = 0;
  conn->challenge_header = NULL;
  conn->response_header = NULL;
#endif

  if(Curl_multi_pipeline_enabled(data->multi) &&
      !conn->master_buffer) {
    /* Allocate master_buffer to be used for pipelining */
    conn->master_buffer = calloc(BUFSIZE, sizeof (char));
    if(!conn->master_buffer)
      goto error;
  }

  /* Initialize the pipeline lists */
  conn->send_pipe = Curl_llist_alloc((curl_llist_dtor) llist_dtor);
  conn->recv_pipe = Curl_llist_alloc((curl_llist_dtor) llist_dtor);
  if(!conn->send_pipe || !conn->recv_pipe)
    goto error;

#ifdef HAVE_GSSAPI
  conn->data_prot = PROT_CLEAR;
#endif

  /* Store the local bind parameters that will be used for this connection */
  if(data->set.str[STRING_DEVICE]) {
    conn->localdev = strdup(data->set.str[STRING_DEVICE]);
    if(!conn->localdev)
      goto error;
  }
  conn->localportrange = data->set.localportrange;
  conn->localport = data->set.localport;

  /* the close socket stuff needs to be copied to the connection struct as
     it may live on without (this specific) SessionHandle */
  conn->fclosesocket = data->set.fclosesocket;
  conn->closesocket_client = data->set.closesocket_client;

  return conn;
  error:

  Curl_llist_destroy(conn->send_pipe, NULL);
  Curl_llist_destroy(conn->recv_pipe, NULL);

  conn->send_pipe = NULL;
  conn->recv_pipe = NULL;

  Curl_safefree(conn->master_buffer);
  Curl_safefree(conn->localdev);
  Curl_safefree(conn);
  return NULL;
}