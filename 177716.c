sign(gcry_mpi_t a, gcry_mpi_t b, gcry_mpi_t input, ELG_secret_key *skey )
{
    gcry_mpi_t k;
    gcry_mpi_t t   = mpi_alloc( mpi_get_nlimbs(a) );
    gcry_mpi_t inv = mpi_alloc( mpi_get_nlimbs(a) );
    gcry_mpi_t p_1 = mpi_copy(skey->p);

   /*
    * b = (t * inv) mod (p-1)
    * b = (t * inv(k,(p-1),(p-1)) mod (p-1)
    * b = (((M-x*a) mod (p-1)) * inv(k,(p-1),(p-1))) mod (p-1)
    *
    */
    mpi_sub_ui(p_1, p_1, 1);
    k = gen_k( skey->p, 0 /* no small K ! */ );
    mpi_powm( a, skey->g, k, skey->p );
    mpi_mul(t, skey->x, a );
    mpi_subm(t, input, t, p_1 );
    mpi_invm(inv, k, p_1 );
    mpi_mulm(b, t, inv, p_1 );

#if 0
    if( DBG_CIPHER )
      {
	log_mpidump ("elg sign p", skey->p);
	log_mpidump ("elg sign g", skey->g);
	log_mpidump ("elg sign y", skey->y);
	log_mpidump ("elg sign x", skey->x);
	log_mpidump ("elg sign k", k);
	log_mpidump ("elg sign M", input);
	log_mpidump ("elg sign a", a);
	log_mpidump ("elg sign b", b);
      }
#endif
    mpi_free(k);
    mpi_free(t);
    mpi_free(inv);
    mpi_free(p_1);
}