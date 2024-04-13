do_fast_random_poll (void)
{
  gcry_assert (pool_is_locked);

  rndstats.fastpolls++;

  if (fast_gather_fnc)
    fast_gather_fnc (add_randomness, RANDOM_ORIGIN_FASTPOLL);

  /* Continue with the generic functions. */
#if HAVE_GETHRTIME
  {
    hrtime_t tv;
    tv = gethrtime();
    add_randomness( &tv, sizeof(tv), RANDOM_ORIGIN_FASTPOLL );
  }
#elif HAVE_GETTIMEOFDAY
  {
    struct timeval tv;
    if( gettimeofday( &tv, NULL ) )
      BUG();
    add_randomness( &tv.tv_sec, sizeof(tv.tv_sec), RANDOM_ORIGIN_FASTPOLL );
    add_randomness( &tv.tv_usec, sizeof(tv.tv_usec), RANDOM_ORIGIN_FASTPOLL );
  }
#elif HAVE_CLOCK_GETTIME
  {	struct timespec tv;
  if( clock_gettime( CLOCK_REALTIME, &tv ) == -1 )
    BUG();
  add_randomness( &tv.tv_sec, sizeof(tv.tv_sec), RANDOM_ORIGIN_FASTPOLL );
  add_randomness( &tv.tv_nsec, sizeof(tv.tv_nsec), RANDOM_ORIGIN_FASTPOLL );
  }
#else /* use times */
# ifndef HAVE_DOSISH_SYSTEM
  {	struct tms buf;
  times( &buf );
  add_randomness( &buf, sizeof buf, RANDOM_ORIGIN_FASTPOLL );
  }
# endif
#endif

#ifdef HAVE_GETRUSAGE
# ifdef RUSAGE_SELF
  {
    struct rusage buf;
    /* QNX/Neutrino does return ENOSYS - so we just ignore it and add
       whatever is in buf.  In a chroot environment it might not work
       at all (i.e. because /proc/ is not accessible), so we better
       ignore all error codes and hope for the best. */
    getrusage (RUSAGE_SELF, &buf );
    add_randomness( &buf, sizeof buf, RANDOM_ORIGIN_FASTPOLL );
    memset( &buf, 0, sizeof buf );
  }
# else /*!RUSAGE_SELF*/
#  ifdef __GCC__
#   warning There is no RUSAGE_SELF on this system
#  endif
# endif /*!RUSAGE_SELF*/
#endif /*HAVE_GETRUSAGE*/

  /* Time and clock are available on all systems - so we better do it
     just in case one of the above functions didn't work.  */
  {
    time_t x = time(NULL);
    add_randomness( &x, sizeof(x), RANDOM_ORIGIN_FASTPOLL );
  }
  {
    clock_t x = clock();
    add_randomness( &x, sizeof(x), RANDOM_ORIGIN_FASTPOLL );
  }

  /* If the system features a fast hardware RNG, read some bytes from
     there.  */
  _gcry_rndhw_poll_fast (add_randomness, RANDOM_ORIGIN_FASTPOLL);
}