mbed_connect_step3(struct connectdata *conn,
                   int sockindex)
{
  CURLcode retcode = CURLE_OK;
  struct ssl_connect_data *connssl = &conn->ssl[sockindex];
  struct SessionHandle *data = conn->data;
  void *old_ssl_sessionid = NULL;
  mbedtls_ssl_session *our_ssl_sessionid;
  int ret;

  DEBUGASSERT(ssl_connect_3 == connssl->connecting_state);

  our_ssl_sessionid = malloc(sizeof(mbedtls_ssl_session));
  if(!our_ssl_sessionid)
    return CURLE_OUT_OF_MEMORY;

  mbedtls_ssl_session_init(our_ssl_sessionid);

  ret = mbedtls_ssl_get_session(&connssl->ssl, our_ssl_sessionid);
  if(ret) {
    failf(data, "mbedtls_ssl_get_session returned -0x%x", -ret);
    return CURLE_SSL_CONNECT_ERROR;
  }

  /* If there's already a matching session in the cache, delete it */
  if(!Curl_ssl_getsessionid(conn, &old_ssl_sessionid, NULL))
    Curl_ssl_delsessionid(conn, old_ssl_sessionid);

  retcode = Curl_ssl_addsessionid(conn, our_ssl_sessionid, 0);
  if(retcode) {
    free(our_ssl_sessionid);
    failf(data, "failed to store ssl session");
    return retcode;
  }

  connssl->connecting_state = ssl_connect_done;

  return CURLE_OK;
}