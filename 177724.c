gen_k( gcry_mpi_t p, int small_k )
{
  gcry_mpi_t k = mpi_alloc_secure( 0 );
  gcry_mpi_t temp = mpi_alloc( mpi_get_nlimbs(p) );
  gcry_mpi_t p_1 = mpi_copy(p);
  unsigned int orig_nbits = mpi_get_nbits(p);
  unsigned int nbits, nbytes;
  char *rndbuf = NULL;

  if (small_k)
    {
      /* Using a k much lesser than p is sufficient for encryption and
       * it greatly improves the encryption performance.  We use
       * Wiener's table and add a large safety margin. */
      nbits = wiener_map( orig_nbits ) * 3 / 2;
      if( nbits >= orig_nbits )
        BUG();
    }
  else
    nbits = orig_nbits;


  nbytes = (nbits+7)/8;
  if( DBG_CIPHER )
    log_debug("choosing a random k\n");
  mpi_sub_ui( p_1, p, 1);
  for(;;)
    {
      if( !rndbuf || nbits < 32 )
        {
          xfree(rndbuf);
          rndbuf = _gcry_random_bytes_secure( nbytes, GCRY_STRONG_RANDOM );
        }
      else
        {
          /* Change only some of the higher bits.  We could improve
             this by directly requesting more memory at the first call
             to get_random_bytes() and use this the here maybe it is
             easier to do this directly in random.c Anyway, it is
             highly inlikely that we will ever reach this code. */
          char *pp = _gcry_random_bytes_secure( 4, GCRY_STRONG_RANDOM );
          memcpy( rndbuf, pp, 4 );
          xfree(pp);
	}
      _gcry_mpi_set_buffer( k, rndbuf, nbytes, 0 );

      for(;;)
        {
          if( !(mpi_cmp( k, p_1 ) < 0) )  /* check: k < (p-1) */
            {
              if( DBG_CIPHER )
                progress('+');
              break; /* no  */
            }
          if( !(mpi_cmp_ui( k, 0 ) > 0) )  /* check: k > 0 */
            {
              if( DBG_CIPHER )
                progress('-');
              break; /* no */
            }
          if (mpi_gcd( temp, k, p_1 ))
            goto found;  /* okay, k is relative prime to (p-1) */
          mpi_add_ui( k, k, 1 );
          if( DBG_CIPHER )
            progress('.');
	}
    }
 found:
  xfree (rndbuf);
  if( DBG_CIPHER )
    progress('\n');
  mpi_free(p_1);
  mpi_free(temp);

  return k;
}