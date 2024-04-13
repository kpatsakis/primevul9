read_pool (byte *buffer, size_t length, int level)
{
  int i;
  unsigned long *sp, *dp;
  /* The volatile is there to make sure the compiler does not optimize
     the code away in case the getpid function is badly attributed.
     Note that we keep a pid in a static variable as well as in a
     stack based one; the latter is to detect ill behaving thread
     libraries, ignoring the pool mutexes. */
  static volatile pid_t my_pid = (pid_t)(-1);
  volatile pid_t my_pid2;

  gcry_assert (pool_is_locked);

 retry:
  /* Get our own pid, so that we can detect a fork. */
  my_pid2 = getpid ();
  if (my_pid == (pid_t)(-1))
    my_pid = my_pid2;
  if ( my_pid != my_pid2 )
    {
      /* We detected a plain fork; i.e. we are now the child.  Update
         the static pid and add some randomness. */
      pid_t x;

      my_pid = my_pid2;
      x = my_pid;
      add_randomness (&x, sizeof(x), RANDOM_ORIGIN_INIT);
      just_mixed = 0; /* Make sure it will get mixed. */
    }

  gcry_assert (pool_is_locked);

  /* Our code does not allow to extract more than POOLSIZE.  Better
     check it here. */
  if (length > POOLSIZE)
    {
      log_bug("too many random bits requested\n");
    }

  if (!pool_filled)
    {
      if (read_seed_file() )
        pool_filled = 1;
    }

  /* For level 2 quality (key generation) we always make sure that the
     pool has been seeded enough initially. */
  if (level == GCRY_VERY_STRONG_RANDOM && !did_initial_extra_seeding)
    {
      size_t needed;

      pool_balance = 0;
      needed = length - pool_balance;
      if (needed < 16)  /* At least 128 bits.  */
        needed = 16;
      else if( needed > POOLSIZE )
        BUG ();
      read_random_source (RANDOM_ORIGIN_EXTRAPOLL, needed,
                          GCRY_VERY_STRONG_RANDOM);
      pool_balance += needed;
      did_initial_extra_seeding = 1;
    }

  /* For level 2 make sure that there is enough random in the pool. */
  if (level == GCRY_VERY_STRONG_RANDOM && pool_balance < length)
    {
      size_t needed;

      if (pool_balance < 0)
        pool_balance = 0;
      needed = length - pool_balance;
      if (needed > POOLSIZE)
        BUG ();
      read_random_source (RANDOM_ORIGIN_EXTRAPOLL, needed,
                          GCRY_VERY_STRONG_RANDOM);
      pool_balance += needed;
    }

  /* Make sure the pool is filled. */
  while (!pool_filled)
    random_poll();

  /* Always do a fast random poll (we have to use the unlocked version). */
  do_fast_random_poll();

  /* Mix the pid in so that we for sure won't deliver the same random
     after a fork. */
  {
    pid_t apid = my_pid;
    add_randomness (&apid, sizeof (apid), RANDOM_ORIGIN_INIT);
  }

  /* Mix the pool (if add_randomness() didn't it). */
  if (!just_mixed)
    {
      mix_pool(rndpool);
      rndstats.mixrnd++;
    }

  /* Create a new pool. */
  for(i=0,dp=(unsigned long*)(void*)keypool, sp=(unsigned long*)(void*)rndpool;
      i < POOLWORDS; i++, dp++, sp++ )
    *dp = *sp + ADD_VALUE;

  /* Mix both pools. */
  mix_pool(rndpool); rndstats.mixrnd++;
  mix_pool(keypool); rndstats.mixkey++;

  /* Read the requested data.  We use a read pointer to read from a
     different position each time.  */
  while (length--)
    {
      *buffer++ = keypool[pool_readpos++];
      if (pool_readpos >= POOLSIZE)
        pool_readpos = 0;
      pool_balance--;
    }

  if (pool_balance < 0)
    pool_balance = 0;

  /* Clear the keypool. */
  memset (keypool, 0, POOLSIZE);

  /* We need to detect whether a fork has happened.  A fork might have
     an identical pool and thus the child and the parent could emit
     the very same random number.  This test here is to detect forks
     in a multi-threaded process.  It does not work with all thread
     implementations in particular not with pthreads.  However it is
     good enough for GNU Pth. */
  if ( getpid () != my_pid2 )
    {
      pid_t x = getpid();
      add_randomness (&x, sizeof(x), RANDOM_ORIGIN_INIT);
      just_mixed = 0; /* Make sure it will get mixed. */
      my_pid = x;     /* Also update the static pid. */
      goto retry;
    }
}