static CURLcode create_conn_helper_init_proxy(struct connectdata *conn)
{
  char *proxy = NULL;
  char *socksproxy = NULL;
  char *no_proxy = NULL;
  CURLcode result = CURLE_OK;
  struct Curl_easy *data = conn->data;

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

  if(data->set.str[STRING_PRE_PROXY]) {
    socksproxy = strdup(data->set.str[STRING_PRE_PROXY]);
    /* if global socks proxy is set, this is it */
    if(NULL == socksproxy) {
      failf(data, "memory shortage");
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }

  if(!data->set.str[STRING_NOPROXY]) {
    no_proxy = curl_getenv("no_proxy");
    if(!no_proxy)
      no_proxy = curl_getenv("NO_PROXY");
  }

  if(check_noproxy(conn->host.name, data->set.str[STRING_NOPROXY] ?
      data->set.str[STRING_NOPROXY] : no_proxy)) {
    Curl_safefree(proxy);
    Curl_safefree(socksproxy);
  }
#ifndef CURL_DISABLE_HTTP
  else if(!proxy && !socksproxy)
    /* if the host is not in the noproxy list, detect proxy. */
    proxy = detect_proxy(conn);
#endif /* CURL_DISABLE_HTTP */

  Curl_safefree(no_proxy);

#ifdef USE_UNIX_SOCKETS
  /* For the time being do not mix proxy and unix domain sockets. See #1274 */
  if(proxy && conn->unix_domain_socket) {
    free(proxy);
    proxy = NULL;
  }
#endif

  if(proxy && (!*proxy || (conn->handler->flags & PROTOPT_NONETWORK))) {
    free(proxy);  /* Don't bother with an empty proxy string or if the
                     protocol doesn't work with network */
    proxy = NULL;
  }
  if(socksproxy && (!*socksproxy ||
                    (conn->handler->flags & PROTOPT_NONETWORK))) {
    free(socksproxy);  /* Don't bother with an empty socks proxy string or if
                          the protocol doesn't work with network */
    socksproxy = NULL;
  }

  /***********************************************************************
   * If this is supposed to use a proxy, we need to figure out the proxy host
   * name, proxy type and port number, so that we can re-use an existing
   * connection that may exist registered to the same proxy host.
   ***********************************************************************/
  if(proxy || socksproxy) {
    if(proxy) {
      result = parse_proxy(data, conn, proxy, conn->http_proxy.proxytype);
      Curl_safefree(proxy); /* parse_proxy copies the proxy string */
      if(result)
        goto out;
    }

    if(socksproxy) {
      result = parse_proxy(data, conn, socksproxy,
                           conn->socks_proxy.proxytype);
      /* parse_proxy copies the socks proxy string */
      Curl_safefree(socksproxy);
      if(result)
        goto out;
    }

    if(conn->http_proxy.host.rawalloc) {
#ifdef CURL_DISABLE_HTTP
      /* asking for a HTTP proxy is a bit funny when HTTP is disabled... */
      result = CURLE_UNSUPPORTED_PROTOCOL;
      goto out;
#else
      /* force this connection's protocol to become HTTP if compatible */
      if(!(conn->handler->protocol & PROTO_FAMILY_HTTP)) {
        if((conn->handler->flags & PROTOPT_PROXY_AS_HTTP) &&
           !conn->bits.tunnel_proxy)
          conn->handler = &Curl_handler_http;
        else
          /* if not converting to HTTP over the proxy, enforce tunneling */
          conn->bits.tunnel_proxy = TRUE;
      }
      conn->bits.httpproxy = TRUE;
#endif
    }
    else {
      conn->bits.httpproxy = FALSE; /* not a HTTP proxy */
      conn->bits.tunnel_proxy = FALSE; /* no tunneling if not HTTP */
    }

    if(conn->socks_proxy.host.rawalloc) {
      if(!conn->http_proxy.host.rawalloc) {
        /* once a socks proxy */
        if(!conn->socks_proxy.user) {
          conn->socks_proxy.user = conn->http_proxy.user;
          conn->http_proxy.user = NULL;
          Curl_safefree(conn->socks_proxy.passwd);
          conn->socks_proxy.passwd = conn->http_proxy.passwd;
          conn->http_proxy.passwd = NULL;
        }
      }
      conn->bits.socksproxy = TRUE;
    }
    else
      conn->bits.socksproxy = FALSE; /* not a socks proxy */
  }
  else {
    conn->bits.socksproxy = FALSE;
    conn->bits.httpproxy = FALSE;
  }
  conn->bits.proxy = conn->bits.httpproxy || conn->bits.socksproxy;

  if(!conn->bits.proxy) {
    /* we aren't using the proxy after all... */
    conn->bits.proxy = FALSE;
    conn->bits.httpproxy = FALSE;
    conn->bits.socksproxy = FALSE;
    conn->bits.proxy_user_passwd = FALSE;
    conn->bits.tunnel_proxy = FALSE;
  }

out:

  free(socksproxy);
  free(proxy);
  return result;
}