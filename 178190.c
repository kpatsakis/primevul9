getfnc_gather_random (void))(void (*)(const void*, size_t,
                                      enum random_origins),
                             enum random_origins, size_t, int)
{
  int (*fnc)(void (*)(const void*, size_t, enum random_origins),
             enum random_origins, size_t, int);

#if USE_RNDLINUX
  if ( !access (NAME_OF_DEV_RANDOM, R_OK)
       && !access (NAME_OF_DEV_URANDOM, R_OK))
    {
      fnc = _gcry_rndlinux_gather_random;
      return fnc;
    }
#endif

#if USE_RNDEGD
  if ( _gcry_rndegd_connect_socket (1) != -1 )
    {
      fnc = _gcry_rndegd_gather_random;
      return fnc;
    }
#endif

#if USE_RNDUNIX
  fnc = _gcry_rndunix_gather_random;
  return fnc;
#endif

#if USE_RNDW32
  fnc = _gcry_rndw32_gather_random;
  return fnc;
#endif

#if USE_RNDW32CE
  fnc = _gcry_rndw32ce_gather_random;
  return fnc;
#endif

  log_fatal (_("no entropy gathering module detected\n"));

  return NULL; /*NOTREACHED*/
}