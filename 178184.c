getfnc_fast_random_poll (void))( void (*)(const void*, size_t,
                                          enum random_origins),
                                 enum random_origins)
{
#if USE_RNDW32
  return _gcry_rndw32_gather_random_fast;
#endif
#if USE_RNDW32CE
  return _gcry_rndw32ce_gather_random_fast;
#endif
  return NULL;
}