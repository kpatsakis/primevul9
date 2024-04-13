static ssize_t mbed_send(struct connectdata *conn, int sockindex,
                         const void *mem, size_t len,
                         CURLcode *curlcode)
{
  int ret = -1;

  ret = mbedtls_ssl_write(&conn->ssl[sockindex].ssl,
                          (unsigned char *)mem, len);

  if(ret < 0) {
    *curlcode = (ret == MBEDTLS_ERR_SSL_WANT_WRITE) ?
      CURLE_AGAIN : CURLE_SEND_ERROR;
    ret = -1;
  }

  return ret;
}