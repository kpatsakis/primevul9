_gcry_rngcsprng_add_bytes (const void *buf, size_t buflen, int quality)
{
  size_t nbytes;
  const char *bufptr;

  if (quality == -1)
    quality = 35;
  else if (quality > 100)
    quality = 100;
  else if (quality < 0)
    quality = 0;

  if (!buf)
    return gpg_error (GPG_ERR_INV_ARG);

  if (!buflen || quality < 10)
    return 0; /* Take a shortcut. */

  /* Because we don't increment the entropy estimation with FASTPOLL,
     we don't need to take lock that estimation while adding from an
     external source.  This limited entropy estimation also means that
     we can't take QUALITY into account.  */
  initialize_basics ();
  bufptr = buf;
  while (buflen)
    {
      nbytes = buflen > POOLSIZE? POOLSIZE : buflen;
      lock_pool ();
      if (rndpool)
        add_randomness (bufptr, nbytes, RANDOM_ORIGIN_EXTERNAL);
      unlock_pool ();
      bufptr += nbytes;
      buflen -= nbytes;
    }
  return 0;
}