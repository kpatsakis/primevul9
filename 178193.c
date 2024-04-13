_gcry_rngcsprng_randomize (void *buffer, size_t length,
                           enum gcry_random_level level)
{
  unsigned char *p;

  /* Make sure we are initialized. */
  initialize ();

  /* Handle our hack used for regression tests of Libgcrypt. */
  if ( quick_test && level > GCRY_STRONG_RANDOM )
    level = GCRY_STRONG_RANDOM;

  /* Make sure the level is okay. */
  level &= 3;

#ifdef USE_RANDOM_DAEMON
  if (allow_daemon
      && !_gcry_daemon_randomize (daemon_socket_name, buffer, length, level))
    return; /* The daemon succeeded. */
  allow_daemon = 0; /* Daemon failed - switch off. */
#endif /*USE_RANDOM_DAEMON*/

  /* Acquire the pool lock. */
  lock_pool ();

  /* Update the statistics. */
  if (level >= GCRY_VERY_STRONG_RANDOM)
    {
      rndstats.getbytes2 += length;
      rndstats.ngetbytes2++;
    }
  else
    {
      rndstats.getbytes1 += length;
      rndstats.ngetbytes1++;
    }

  /* Read the random into the provided buffer. */
  for (p = buffer; length > 0;)
    {
      size_t n;

      n = length > POOLSIZE? POOLSIZE : length;
      read_pool (p, n, level);
      length -= n;
      p += n;
    }

  /* Release the pool lock. */
  unlock_pool ();
}