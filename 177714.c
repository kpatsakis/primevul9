test_keys ( ELG_secret_key *sk, unsigned int nbits, int nodie )
{
  ELG_public_key pk;
  gcry_mpi_t test   = mpi_new ( 0 );
  gcry_mpi_t out1_a = mpi_new ( nbits );
  gcry_mpi_t out1_b = mpi_new ( nbits );
  gcry_mpi_t out2   = mpi_new ( nbits );
  int failed = 0;

  pk.p = sk->p;
  pk.g = sk->g;
  pk.y = sk->y;

  _gcry_mpi_randomize ( test, nbits, GCRY_WEAK_RANDOM );

  do_encrypt ( out1_a, out1_b, test, &pk );
  decrypt ( out2, out1_a, out1_b, sk );
  if ( mpi_cmp( test, out2 ) )
    failed |= 1;

  sign ( out1_a, out1_b, test, sk );
  if ( !verify( out1_a, out1_b, test, &pk ) )
    failed |= 2;

  _gcry_mpi_release ( test );
  _gcry_mpi_release ( out1_a );
  _gcry_mpi_release ( out1_b );
  _gcry_mpi_release ( out2 );

  if (failed && !nodie)
    log_fatal ("Elgamal test key for %s %s failed\n",
               (failed & 1)? "encrypt+decrypt":"",
               (failed & 2)? "sign+verify":"");
  if (failed && DBG_CIPHER)
    log_debug ("Elgamal test key for %s %s failed\n",
               (failed & 1)? "encrypt+decrypt":"",
               (failed & 2)? "sign+verify":"");

  return failed;
}