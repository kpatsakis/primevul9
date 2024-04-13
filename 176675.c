void Curl_mbedtls_cleanup(void)
{
  (void)Curl_polarsslthreadlock_thread_cleanup();
}