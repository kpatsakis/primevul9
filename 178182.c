initialize(void)
{
  /* Although the basic initialization should have happened already,
     we call it here to make sure that all prerequisites are met.  */
  initialize_basics ();

  /* Now we can look the pool and complete the initialization if
     necessary.  */
  lock_pool ();
  if (!rndpool)
    {
      /* The data buffer is allocated somewhat larger, so that we can
         use this extra space (which is allocated in secure memory) as
         a temporary hash buffer */
      rndpool = (secure_alloc
                 ? xcalloc_secure (1, POOLSIZE + BLOCKLEN)
                 : xcalloc (1, POOLSIZE + BLOCKLEN));
      keypool = (secure_alloc
                 ? xcalloc_secure (1, POOLSIZE + BLOCKLEN)
                 : xcalloc (1, POOLSIZE + BLOCKLEN));

      /* Setup the slow entropy gathering function.  The code requires
         that this function exists. */
      slow_gather_fnc = getfnc_gather_random ();

      /* Setup the fast entropy gathering function.  */
      fast_gather_fnc = getfnc_fast_random_poll ();

    }
  unlock_pool ();
}