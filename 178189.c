_gcry_rngcsprng_close_fds (void)
{
  lock_pool ();
#if USE_RNDLINUX
  _gcry_rndlinux_gather_random (NULL, 0, 0, 0);
  pool_filled = 0; /* Force re-open on next use.  */
#endif
  unlock_pool ();
}