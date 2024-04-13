Curl_polarssl_connect_nonblocking(struct connectdata *conn,
                                int sockindex,
                                bool *done)
{
  return polarssl_connect_common(conn, sockindex, TRUE, done);
}