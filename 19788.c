static CURLcode create_conn(struct SessionHandle *data,
                            struct connectdata **in_connect,
                            bool *async)
{
  CURLcode result = CURLE_OK;
  struct connectdata *conn;
  struct connectdata *conn_temp = NULL;
  size_t urllen;
  char *user = NULL;
  char *passwd = NULL;
  char *options = NULL;
  bool reuse;
  char *proxy = NULL;
  bool prot_missing = FALSE;
  bool no_connections_available = FALSE;
  bool force_reuse = FALSE;
  size_t max_host_connections = Curl_multi_max_host_connections(data->multi);
  size_t max_total_connections = Curl_multi_max_total_connections(data->multi);

  *async = FALSE;

  /*************************************************************
   * Check input data
   *************************************************************/

  if(!data->change.url) {
    result = CURLE_URL_MALFORMAT;
    goto out;
  }

  /* First, split up the current URL in parts so that we can use the
     parts for checking against the already present connections. In order
     to not have to modify everything at once, we allocate a temporary
     connection data struct and fill in for comparison purposes. */
  conn = allocate_conn(data);

  if(!conn) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  /* We must set the return variable as soon as possible, so that our
     parent can cleanup any possible allocs we may have done before
     any failure */
  *in_connect = conn;

  /* This initing continues below, see the comment "Continue connectdata
   * initialization here" */

  /***********************************************************
   * We need to allocate memory to store the path in. We get the size of the
   * full URL to be sure, and we need to make it at least 256 bytes since
   * other parts of the code will rely on this fact
   ***********************************************************/
#define LEAST_PATH_ALLOC 256
  urllen=strlen(data->change.url);
  if(urllen < LEAST_PATH_ALLOC)
    urllen=LEAST_PATH_ALLOC;

  /*
   * We malloc() the buffers below urllen+2 to make room for 2 possibilities:
   * 1 - an extra terminating zero
   * 2 - an extra slash (in case a syntax like "www.host.com?moo" is used)
   */

  Curl_safefree(data->state.pathbuffer);
  data->state.path = NULL;

  data->state.pathbuffer = malloc(urllen+2);
  if(NULL == data->state.pathbuffer) {
    result = CURLE_OUT_OF_MEMORY; /* really bad error */
    goto out;
  }
  data->state.path = data->state.pathbuffer;

  conn->host.rawalloc = malloc(urllen+2);
  if(NULL == conn->host.rawalloc) {
    Curl_safefree(data->state.pathbuffer);
    data->state.path = NULL;
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  conn->host.name = conn->host.rawalloc;
  conn->host.name[0] = 0;

  user = strdup("");
  passwd = strdup("");
  options = strdup("");
  if(!user || !passwd || !options) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = parseurlandfillconn(data, conn, &prot_missing, &user, &passwd,
                               &options);
  if(result)
    goto out;

  /*************************************************************
   * No protocol part in URL was used, add it!
   *************************************************************/
  if(prot_missing) {
    /* We're guessing prefixes here and if we're told to use a proxy or if
       we're gonna follow a Location: later or... then we need the protocol
       part added so that we have a valid URL. */
    char *reurl;

    reurl = aprintf("%s://%s", conn->handler->scheme, data->change.url);

    if(!reurl) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }

    if(data->change.url_alloc) {
      Curl_safefree(data->change.url);
      data->change.url_alloc = FALSE;
    }

    data->change.url = reurl;
    data->change.url_alloc = TRUE; /* free this later */
  }

  /*************************************************************
   * If the protocol can't handle url query strings, then cut
   * off the unhandable part
   *************************************************************/
  if((conn->given->flags&PROTOPT_NOURLQUERY)) {
    char *path_q_sep = strchr(conn->data->state.path, '?');
    if(path_q_sep) {
      /* according to rfc3986, allow the query (?foo=bar)
         also on protocols that can't handle it.

         cut the string-part after '?'
      */

      /* terminate the string */
      path_q_sep[0] = 0;
    }
  }

  if(data->set.str[STRING_BEARER]) {
    conn->xoauth2_bearer = strdup(data->set.str[STRING_BEARER]);
    if(!conn->xoauth2_bearer) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }

#ifndef CURL_DISABLE_PROXY
  /*************************************************************
   * Extract the user and password from the authentication string
   *************************************************************/
  if(conn->bits.proxy_user_passwd) {
    result = parse_proxy_auth(data, conn);
    if(result)
      goto out;
  }

  /*************************************************************
   * Detect what (if any) proxy to use
   *************************************************************/
  if(data->set.str[STRING_PROXY]) {
    proxy = strdup(data->set.str[STRING_PROXY]);
    /* if global proxy is set, this is it */
    if(NULL == proxy) {
      failf(data, "memory shortage");
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }

  if(data->set.str[STRING_NOPROXY] &&
     check_noproxy(conn->host.name, data->set.str[STRING_NOPROXY])) {
    if(proxy) {
      free(proxy);  /* proxy is in exception list */
      proxy = NULL;
    }
  }
  else if(!proxy)
    proxy = detect_proxy(conn);

#ifdef USE_UNIX_SOCKETS
  if(proxy && data->set.str[STRING_UNIX_SOCKET_PATH]) {
    free(proxy);  /* Unix domain sockets cannot be proxied, so disable it */
    proxy = NULL;
  }
#endif

  if(proxy && (!*proxy || (conn->handler->flags & PROTOPT_NONETWORK))) {
    free(proxy);  /* Don't bother with an empty proxy string or if the
                     protocol doesn't work with network */
    proxy = NULL;
  }

  /***********************************************************************
   * If this is supposed to use a proxy, we need to figure out the proxy host
   * name, proxy type and port number, so that we can re-use an existing
   * connection that may exist registered to the same proxy host.
   ***********************************************************************/
  if(proxy) {
    result = parse_proxy(data, conn, proxy);

    Curl_safefree(proxy); /* parse_proxy copies the proxy string */

    if(result)
      goto out;

    if((conn->proxytype == CURLPROXY_HTTP) ||
       (conn->proxytype == CURLPROXY_HTTP_1_0)) {
#ifdef CURL_DISABLE_HTTP
      /* asking for a HTTP proxy is a bit funny when HTTP is disabled... */
      result = CURLE_UNSUPPORTED_PROTOCOL;
      goto out;
#else
      /* force this connection's protocol to become HTTP if not already
         compatible - if it isn't tunneling through */
      if(!(conn->handler->protocol & PROTO_FAMILY_HTTP) &&
         !conn->bits.tunnel_proxy)
        conn->handler = &Curl_handler_http;

      conn->bits.httpproxy = TRUE;
#endif
    }
    else
      conn->bits.httpproxy = FALSE; /* not a HTTP proxy */
    conn->bits.proxy = TRUE;
  }
  else {
    /* we aren't using the proxy after all... */
    conn->bits.proxy = FALSE;
    conn->bits.httpproxy = FALSE;
    conn->bits.proxy_user_passwd = FALSE;
    conn->bits.tunnel_proxy = FALSE;
  }

#endif /* CURL_DISABLE_PROXY */

  /*************************************************************
   * If the protocol is using SSL and HTTP proxy is used, we set
   * the tunnel_proxy bit.
   *************************************************************/
  if((conn->given->flags&PROTOPT_SSL) && conn->bits.httpproxy)
    conn->bits.tunnel_proxy = TRUE;

  /*************************************************************
   * Figure out the remote port number and fix it in the URL
   *************************************************************/
  result = parse_remote_port(data, conn);
  if(result)
    goto out;

  /* Check for overridden login details and set them accordingly so they
     they are known when protocol->setup_connection is called! */
  result = override_login(data, conn, &user, &passwd, &options);
  if(result)
    goto out;
  result = set_login(conn, user, passwd, options);
  if(result)
    goto out;

  /*************************************************************
   * Setup internals depending on protocol. Needs to be done after
   * we figured out what/if proxy to use.
   *************************************************************/
  result = setup_connection_internals(conn);
  if(result)
    goto out;

  conn->recv[FIRSTSOCKET] = Curl_recv_plain;
  conn->send[FIRSTSOCKET] = Curl_send_plain;
  conn->recv[SECONDARYSOCKET] = Curl_recv_plain;
  conn->send[SECONDARYSOCKET] = Curl_send_plain;

  /***********************************************************************
   * file: is a special case in that it doesn't need a network connection
   ***********************************************************************/
#ifndef CURL_DISABLE_FILE
  if(conn->handler->flags & PROTOPT_NONETWORK) {
    bool done;
    /* this is supposed to be the connect function so we better at least check
       that the file is present here! */
    DEBUGASSERT(conn->handler->connect_it);
    result = conn->handler->connect_it(conn, &done);

    /* Setup a "faked" transfer that'll do nothing */
    if(!result) {
      conn->data = data;
      conn->bits.tcpconnect[FIRSTSOCKET] = TRUE; /* we are "connected */

      ConnectionStore(data, conn);

      /*
       * Setup whatever necessary for a resumed transfer
       */
      result = setup_range(data);
      if(result) {
        DEBUGASSERT(conn->handler->done);
        /* we ignore the return code for the protocol-specific DONE */
        (void)conn->handler->done(conn, result, FALSE);
        goto out;
      }

      Curl_setup_transfer(conn, -1, -1, FALSE, NULL, /* no download */
                          -1, NULL); /* no upload */
    }

    /* since we skip do_init() */
    do_init(conn);

    goto out;
  }
#endif

  /* Get a cloned copy of the SSL config situation stored in the
     connection struct. But to get this going nicely, we must first make
     sure that the strings in the master copy are pointing to the correct
     strings in the session handle strings array!

     Keep in mind that the pointers in the master copy are pointing to strings
     that will be freed as part of the SessionHandle struct, but all cloned
     copies will be separately allocated.
  */
  data->set.ssl.CApath = data->set.str[STRING_SSL_CAPATH];
  data->set.ssl.CAfile = data->set.str[STRING_SSL_CAFILE];
  data->set.ssl.CRLfile = data->set.str[STRING_SSL_CRLFILE];
  data->set.ssl.issuercert = data->set.str[STRING_SSL_ISSUERCERT];
  data->set.ssl.random_file = data->set.str[STRING_SSL_RANDOM_FILE];
  data->set.ssl.egdsocket = data->set.str[STRING_SSL_EGDSOCKET];
  data->set.ssl.cipher_list = data->set.str[STRING_SSL_CIPHER_LIST];
#ifdef USE_TLS_SRP
  data->set.ssl.username = data->set.str[STRING_TLSAUTH_USERNAME];
  data->set.ssl.password = data->set.str[STRING_TLSAUTH_PASSWORD];
#endif

  if(!Curl_clone_ssl_config(&data->set.ssl, &conn->ssl_config)) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  prune_dead_connections(data);

  /*************************************************************
   * Check the current list of connections to see if we can
   * re-use an already existing one or if we have to create a
   * new one.
   *************************************************************/

  /* reuse_fresh is TRUE if we are told to use a new connection by force, but
     we only acknowledge this option if this is not a re-used connection
     already (which happens due to follow-location or during a HTTP
     authentication phase). */
  if(data->set.reuse_fresh && !data->state.this_is_a_follow)
    reuse = FALSE;
  else
    reuse = ConnectionExists(data, conn, &conn_temp, &force_reuse);

  /* If we found a reusable connection, we may still want to
     open a new connection if we are pipelining. */
  if(reuse && !force_reuse && IsPipeliningPossible(data, conn_temp)) {
    size_t pipelen = conn_temp->send_pipe->size + conn_temp->recv_pipe->size;
    if(pipelen > 0) {
      infof(data, "Found connection %ld, with requests in the pipe (%zu)\n",
            conn_temp->connection_id, pipelen);

      if(conn_temp->bundle->num_connections < max_host_connections &&
         data->state.conn_cache->num_connections < max_total_connections) {
        /* We want a new connection anyway */
        reuse = FALSE;

        infof(data, "We can reuse, but we want a new connection anyway\n");
      }
    }
  }

  if(reuse) {
    /*
     * We already have a connection for this, we got the former connection
     * in the conn_temp variable and thus we need to cleanup the one we
     * just allocated before we can move along and use the previously
     * existing one.
     */
    conn_temp->inuse = TRUE; /* mark this as being in use so that no other
                                handle in a multi stack may nick it */
    reuse_conn(conn, conn_temp);
    free(conn);          /* we don't need this anymore */
    conn = conn_temp;
    *in_connect = conn;

    /* set a pointer to the hostname we display */
    fix_hostname(data, conn, &conn->host);

    infof(data, "Re-using existing connection! (#%ld) with host %s\n",
          conn->connection_id,
          conn->proxy.name?conn->proxy.dispname:conn->host.dispname);
  }
  else {
    /* We have decided that we want a new connection. However, we may not
       be able to do that if we have reached the limit of how many
       connections we are allowed to open. */
    struct connectbundle *bundle;

    bundle = Curl_conncache_find_bundle(data->state.conn_cache,
                                        conn->host.name);
    if(max_host_connections > 0 && bundle &&
       (bundle->num_connections >= max_host_connections)) {
      struct connectdata *conn_candidate;

      /* The bundle is full. Let's see if we can kill a connection. */
      conn_candidate = find_oldest_idle_connection_in_bundle(data, bundle);

      if(conn_candidate) {
        /* Set the connection's owner correctly, then kill it */
        conn_candidate->data = data;
        (void)Curl_disconnect(conn_candidate, /* dead_connection */ FALSE);
      }
      else
        no_connections_available = TRUE;
    }

    if(max_total_connections > 0 &&
       (data->state.conn_cache->num_connections >= max_total_connections)) {
      struct connectdata *conn_candidate;

      /* The cache is full. Let's see if we can kill a connection. */
      conn_candidate = find_oldest_idle_connection(data);

      if(conn_candidate) {
        /* Set the connection's owner correctly, then kill it */
        conn_candidate->data = data;
        (void)Curl_disconnect(conn_candidate, /* dead_connection */ FALSE);
      }
      else
        no_connections_available = TRUE;
    }


    if(no_connections_available) {
      infof(data, "No connections available.\n");

      conn_free(conn);
      *in_connect = NULL;

      result = CURLE_NO_CONNECTION_AVAILABLE;
      goto out;
    }
    else {
      /*
       * This is a brand new connection, so let's store it in the connection
       * cache of ours!
       */
      ConnectionStore(data, conn);
    }

#if defined(USE_NTLM)
    /* If NTLM is requested in a part of this connection, make sure we don't
       assume the state is fine as this is a fresh connection and NTLM is
       connection based. */
    if((data->state.authhost.picked & (CURLAUTH_NTLM | CURLAUTH_NTLM_WB)) &&
       data->state.authhost.done) {
      infof(data, "NTLM picked AND auth done set, clear picked!\n");
      data->state.authhost.picked = CURLAUTH_NONE;
    }

    if((data->state.authproxy.picked & (CURLAUTH_NTLM | CURLAUTH_NTLM_WB)) &&
       data->state.authproxy.done) {
      infof(data, "NTLM-proxy picked AND auth done set, clear picked!\n");
      data->state.authproxy.picked = CURLAUTH_NONE;
    }
#endif
  }

  /* Mark the connection as used */
  conn->inuse = TRUE;

  /* Setup and init stuff before DO starts, in preparing for the transfer. */
  do_init(conn);

  /*
   * Setup whatever necessary for a resumed transfer
   */
  result = setup_range(data);
  if(result)
    goto out;

  /* Continue connectdata initialization here. */

  /*
   * Inherit the proper values from the urldata struct AFTER we have arranged
   * the persistent connection stuff
   */
  conn->fread_func = data->set.fread_func;
  conn->fread_in = data->set.in;
  conn->seek_func = data->set.seek_func;
  conn->seek_client = data->set.seek_client;

  /*************************************************************
   * Resolve the address of the server or proxy
   *************************************************************/
  result = resolve_server(data, conn, async);

  out:

  Curl_safefree(options);
  Curl_safefree(passwd);
  Curl_safefree(user);
  Curl_safefree(proxy);
  return result;
}