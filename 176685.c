int Curl_mbedtls_data_pending(const struct connectdata *conn, int sockindex)
{
  mbedtls_ssl_context *ssl =
    (mbedtls_ssl_context *)&conn->ssl[sockindex].ssl;
  return ssl->in_msglen != 0;
}