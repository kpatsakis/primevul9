void Curl_polarssl_session_free(void *ptr)
{
  ssl_session_free(ptr);
  free(ptr);
}