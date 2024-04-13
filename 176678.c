static ssize_t polarssl_send(struct connectdata *conn,
                             int sockindex,
                             const void *mem,
                             size_t len,
                             CURLcode *curlcode)
{
  int ret = -1;

  ret = ssl_write(&conn->ssl[sockindex].ssl,
                  (unsigned char *)mem, len);

  if(ret < 0) {
    *curlcode = (ret == POLARSSL_ERR_NET_WANT_WRITE) ?
      CURLE_AGAIN : CURLE_SEND_ERROR;
    ret = -1;
  }

  return ret;
}