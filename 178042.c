void Curl_verboseconnect(struct connectdata *conn)
{
  if(conn->data->set.verbose)
    infof(conn->data, "Connected to %s (%s) port %ld (#%ld)\n",
          conn->bits.socksproxy ? conn->socks_proxy.host.dispname :
          conn->bits.httpproxy ? conn->http_proxy.host.dispname :
          conn->bits.conn_to_host ? conn->conn_to_host.dispname :
          conn->host.dispname,
          conn->ip_addr_str, conn->port, conn->connection_id);
}