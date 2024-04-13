void Curl_polarssl_cleanup(void)
{
  (void)Curl_polarsslthreadlock_thread_cleanup();
}