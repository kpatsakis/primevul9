int Curl_mbedtls_init(void)
{
  return Curl_polarsslthreadlock_thread_setup();
}