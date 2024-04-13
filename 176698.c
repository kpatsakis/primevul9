int Curl_polarssl_init(void)
{
  return Curl_polarsslthreadlock_thread_setup();
}