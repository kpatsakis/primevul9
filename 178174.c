_gcry_rngcsprng_fast_poll (void)
{
  initialize_basics ();

  lock_pool ();
  if (rndpool)
    {
      /* Yes, we are fully initialized. */
      do_fast_random_poll ();
    }
  unlock_pool ();
}