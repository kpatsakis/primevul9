initialize_basics(void)
{
  static int initialized;

  if (!initialized)
    {
      initialized = 1;

#ifdef USE_RANDOM_DAEMON
      _gcry_daemon_initialize_basics ();
#endif /*USE_RANDOM_DAEMON*/

      /* Make sure that we are still using the values we have
         traditionally used for the random levels.  */
      gcry_assert (GCRY_WEAK_RANDOM == 0
                   && GCRY_STRONG_RANDOM == 1
                   && GCRY_VERY_STRONG_RANDOM == 2);
    }
}