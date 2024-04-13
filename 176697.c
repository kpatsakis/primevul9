void Curl_mbedtls_session_free(void *ptr)
{
  mbedtls_ssl_session_free(ptr);
  free(ptr);
}