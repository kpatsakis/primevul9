_gcry_rngcsprng_use_daemon (int onoff)
{
#ifdef USE_RANDOM_DAEMON
  int last;

  /* This is not really thread safe.  However it is expected that this
     function is being called during initialization and at that point
     we are for other reasons not really thread safe.  We do not want
     to lock it because we might eventually decide that this function
     may even be called prior to gcry_check_version.  */
  last = allow_daemon;
  if (onoff != -1)
    allow_daemon = onoff;

  return last;
#else /*!USE_RANDOM_DAEMON*/
  (void)onoff;
  return 0;
#endif /*!USE_RANDOM_DAEMON*/
}