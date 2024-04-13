_gcry_rngcsprng_is_faked (void)
{
  /* We need to initialize due to the runtime determination of
     available entropy gather modules.  */
  initialize();
  return quick_test;
}