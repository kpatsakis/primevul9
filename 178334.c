_gcry_ecc_eddsa_genkey (ECC_secret_key *sk, elliptic_curve_t *E, mpi_ec_t ctx,
                        int flags)
{
  gpg_err_code_t rc;
  int b = 256/8;             /* The only size we currently support.  */
  gcry_mpi_t a, x, y;
  mpi_point_struct Q;
  gcry_random_level_t random_level;
  char *dbuf;
  size_t dlen;
  gcry_buffer_t hvec[1];
  unsigned char *hash_d = NULL;

  point_init (&Q);
  memset (hvec, 0, sizeof hvec);

  if ((flags & PUBKEY_FLAG_TRANSIENT_KEY))
    random_level = GCRY_STRONG_RANDOM;
  else
    random_level = GCRY_VERY_STRONG_RANDOM;

  a = mpi_snew (0);
  x = mpi_new (0);
  y = mpi_new (0);

  /* Generate a secret.  */
  hash_d = xtrymalloc_secure (2*b);
  if (!hash_d)
    {
      rc = gpg_err_code_from_syserror ();
      goto leave;
    }
  dlen = b;
  dbuf = _gcry_random_bytes_secure (dlen, random_level);

  /* Compute the A value.  */
  hvec[0].data = dbuf;
  hvec[0].len = dlen;
  rc = _gcry_md_hash_buffers (GCRY_MD_SHA512, 0, hash_d, hvec, 1);
  if (rc)
    goto leave;
  sk->d = _gcry_mpi_set_opaque (NULL, dbuf, dlen*8);
  dbuf = NULL;
  reverse_buffer (hash_d, 32);  /* Only the first half of the hash.  */
  hash_d[0] = (hash_d[0] & 0x7f) | 0x40;
  hash_d[31] &= 0xf8;
  _gcry_mpi_set_buffer (a, hash_d, 32, 0);
  xfree (hash_d); hash_d = NULL;
  /* log_printmpi ("ecgen         a", a); */

  /* Compute Q.  */
  _gcry_mpi_ec_mul_point (&Q, a, &E->G, ctx);
  if (DBG_CIPHER)
    log_printpnt ("ecgen      pk", &Q, ctx);

  /* Copy the stuff to the key structures. */
  sk->E.model = E->model;
  sk->E.dialect = E->dialect;
  sk->E.p = mpi_copy (E->p);
  sk->E.a = mpi_copy (E->a);
  sk->E.b = mpi_copy (E->b);
  point_init (&sk->E.G);
  point_set (&sk->E.G, &E->G);
  sk->E.n = mpi_copy (E->n);
  sk->E.h = mpi_copy (E->h);
  point_init (&sk->Q);
  point_set (&sk->Q, &Q);

 leave:
  point_free (&Q);
  _gcry_mpi_release (a);
  _gcry_mpi_release (x);
  _gcry_mpi_release (y);
  xfree (hash_d);
  return rc;
}