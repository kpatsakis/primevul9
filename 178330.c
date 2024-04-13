_gcry_ecc_eddsa_verify (gcry_mpi_t input, ECC_public_key *pkey,
                        gcry_mpi_t r_in, gcry_mpi_t s_in, int hashalgo,
                        gcry_mpi_t pk)
{
  int rc;
  mpi_ec_t ctx = NULL;
  int b;
  unsigned int tmp;
  mpi_point_struct Q;          /* Public key.  */
  unsigned char *encpk = NULL; /* Encoded public key.  */
  unsigned int encpklen;
  const void *mbuf, *rbuf;
  unsigned char *tbuf = NULL;
  size_t mlen, rlen;
  unsigned int tlen;
  unsigned char digest[64];
  gcry_buffer_t hvec[3];
  gcry_mpi_t h, s;
  mpi_point_struct Ia, Ib;

  if (!mpi_is_opaque (input) || !mpi_is_opaque (r_in) || !mpi_is_opaque (s_in))
    return GPG_ERR_INV_DATA;
  if (hashalgo != GCRY_MD_SHA512)
    return GPG_ERR_DIGEST_ALGO;

  point_init (&Q);
  point_init (&Ia);
  point_init (&Ib);
  h = mpi_new (0);
  s = mpi_new (0);

  ctx = _gcry_mpi_ec_p_internal_new (pkey->E.model, pkey->E.dialect, 0,
                                     pkey->E.p, pkey->E.a, pkey->E.b);
  b = ctx->nbits/8;
  if (b != 256/8)
    return GPG_ERR_INTERNAL; /* We only support 256 bit. */

  /* Decode and check the public key.  */
  rc = _gcry_ecc_eddsa_decodepoint (pk, ctx, &Q, &encpk, &encpklen);
  if (rc)
    goto leave;
  if (!_gcry_mpi_ec_curve_point (&Q, ctx))
    {
      rc = GPG_ERR_BROKEN_PUBKEY;
      goto leave;
    }
  if (DBG_CIPHER)
    log_printhex ("  e_pk", encpk, encpklen);
  if (encpklen != b)
    {
      rc = GPG_ERR_INV_LENGTH;
      goto leave;
    }

  /* Convert the other input parameters.  */
  mbuf = mpi_get_opaque (input, &tmp);
  mlen = (tmp +7)/8;
  if (DBG_CIPHER)
    log_printhex ("     m", mbuf, mlen);
  rbuf = mpi_get_opaque (r_in, &tmp);
  rlen = (tmp +7)/8;
  if (DBG_CIPHER)
    log_printhex ("     r", rbuf, rlen);
  if (rlen != b)
    {
      rc = GPG_ERR_INV_LENGTH;
      goto leave;
    }

  /* h = H(encodepoint(R) + encodepoint(pk) + m)  */
  hvec[0].data = (char*)rbuf;
  hvec[0].off  = 0;
  hvec[0].len  = rlen;
  hvec[1].data = encpk;
  hvec[1].off  = 0;
  hvec[1].len  = encpklen;
  hvec[2].data = (char*)mbuf;
  hvec[2].off  = 0;
  hvec[2].len  = mlen;
  rc = _gcry_md_hash_buffers (hashalgo, 0, digest, hvec, 3);
  if (rc)
    goto leave;
  reverse_buffer (digest, 64);
  if (DBG_CIPHER)
    log_printhex (" H(R+)", digest, 64);
  _gcry_mpi_set_buffer (h, digest, 64, 0);

  /* According to the paper the best way for verification is:
         encodepoint(sG - h·Q) = encodepoint(r)
     because we don't need to decode R. */
  {
    void *sbuf;
    unsigned int slen;

    sbuf = _gcry_mpi_get_opaque_copy (s_in, &tmp);
    slen = (tmp +7)/8;
    reverse_buffer (sbuf, slen);
    if (DBG_CIPHER)
      log_printhex ("     s", sbuf, slen);
    _gcry_mpi_set_buffer (s, sbuf, slen, 0);
    xfree (sbuf);
    if (slen != b)
      {
        rc = GPG_ERR_INV_LENGTH;
        goto leave;
      }
  }

  _gcry_mpi_ec_mul_point (&Ia, s, &pkey->E.G, ctx);
  _gcry_mpi_ec_mul_point (&Ib, h, &Q, ctx);
  _gcry_mpi_sub (Ib.x, ctx->p, Ib.x);
  _gcry_mpi_ec_add_points (&Ia, &Ia, &Ib, ctx);
  rc = _gcry_ecc_eddsa_encodepoint (&Ia, ctx, s, h, 0, &tbuf, &tlen);
  if (rc)
    goto leave;
  if (tlen != rlen || memcmp (tbuf, rbuf, tlen))
    {
      rc = GPG_ERR_BAD_SIGNATURE;
      goto leave;
    }

  rc = 0;

 leave:
  xfree (encpk);
  xfree (tbuf);
  _gcry_mpi_ec_free (ctx);
  _gcry_mpi_release (s);
  _gcry_mpi_release (h);
  point_free (&Ia);
  point_free (&Ib);
  point_free (&Q);
  return rc;
}