CURLcode Curl_connected_proxy(struct connectdata *conn, int sockindex)
{
  CURLcode result = CURLE_OK;

  if(conn->bits.socksproxy) {
#ifndef CURL_DISABLE_PROXY
    /* for the secondary socket (FTP), use the "connect to host"
     * but ignore the "connect to port" (use the secondary port)
     */
    const char * const host = conn->bits.httpproxy ?
                              conn->http_proxy.host.name :
                              conn->bits.conn_to_host ?
                              conn->conn_to_host.name :
                              sockindex == SECONDARYSOCKET ?
                              conn->secondaryhostname : conn->host.name;
    const int port = conn->bits.httpproxy ? (int)conn->http_proxy.port :
                     sockindex == SECONDARYSOCKET ? conn->secondary_port :
                     conn->bits.conn_to_port ? conn->conn_to_port :
                     conn->remote_port;
    conn->bits.socksproxy_connecting = TRUE;
    switch(conn->socks_proxy.proxytype) {
    case CURLPROXY_SOCKS5:
    case CURLPROXY_SOCKS5_HOSTNAME:
      result = Curl_SOCKS5(conn->socks_proxy.user, conn->socks_proxy.passwd,
                         host, port, sockindex, conn);
      break;

    case CURLPROXY_SOCKS4:
    case CURLPROXY_SOCKS4A:
      result = Curl_SOCKS4(conn->socks_proxy.user, host, port, sockindex,
                           conn);
      break;

    default:
      failf(conn->data, "unknown proxytype option given");
      result = CURLE_COULDNT_CONNECT;
    } /* switch proxytype */
    conn->bits.socksproxy_connecting = FALSE;
#else
  (void)sockindex;
#endif /* CURL_DISABLE_PROXY */
  }

  return result;
}