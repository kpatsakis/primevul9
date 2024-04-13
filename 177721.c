elg_decrypt (gcry_sexp_t *r_plain, gcry_sexp_t s_data, gcry_sexp_t keyparms)
{
  gpg_err_code_t rc;
  struct pk_encoding_ctx ctx;
  gcry_sexp_t l1 = NULL;
  gcry_mpi_t data_a = NULL;
  gcry_mpi_t data_b = NULL;
  ELG_secret_key sk = {NULL, NULL, NULL, NULL};
  gcry_mpi_t plain = NULL;
  unsigned char *unpad = NULL;
  size_t unpadlen = 0;

  _gcry_pk_util_init_encoding_ctx (&ctx, PUBKEY_OP_DECRYPT,
                                   elg_get_nbits (keyparms));

  /* Extract the data.  */
  rc = _gcry_pk_util_preparse_encval (s_data, elg_names, &l1, &ctx);
  if (rc)
    goto leave;
  rc = sexp_extract_param (l1, NULL, "ab", &data_a, &data_b, NULL);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    {
      log_printmpi ("elg_decrypt  d_a", data_a);
      log_printmpi ("elg_decrypt  d_b", data_b);
    }
  if (mpi_is_opaque (data_a) || mpi_is_opaque (data_b))
    {
      rc = GPG_ERR_INV_DATA;
      goto leave;
    }

  /* Extract the key.  */
  rc = sexp_extract_param (keyparms, NULL, "pgyx",
                           &sk.p, &sk.g, &sk.y, &sk.x,
                           NULL);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    {
      log_printmpi ("elg_decrypt    p", sk.p);
      log_printmpi ("elg_decrypt    g", sk.g);
      log_printmpi ("elg_decrypt    y", sk.y);
      if (!fips_mode ())
        log_printmpi ("elg_decrypt    x", sk.x);
    }

  plain = mpi_snew (ctx.nbits);
  decrypt (plain, data_a, data_b, &sk);
  if (DBG_CIPHER)
    log_printmpi ("elg_decrypt  res", plain);

  /* Reverse the encoding and build the s-expression.  */
  switch (ctx.encoding)
    {
    case PUBKEY_ENC_PKCS1:
      rc = _gcry_rsa_pkcs1_decode_for_enc (&unpad, &unpadlen, ctx.nbits, plain);
      mpi_free (plain); plain = NULL;
      if (!rc)
        rc = sexp_build (r_plain, NULL, "(value %b)", (int)unpadlen, unpad);
      break;

    case PUBKEY_ENC_OAEP:
      rc = _gcry_rsa_oaep_decode (&unpad, &unpadlen,
                                  ctx.nbits, ctx.hash_algo, plain,
                                  ctx.label, ctx.labellen);
      mpi_free (plain); plain = NULL;
      if (!rc)
        rc = sexp_build (r_plain, NULL, "(value %b)", (int)unpadlen, unpad);
      break;

    default:
      /* Raw format.  For backward compatibility we need to assume a
         signed mpi by using the sexp format string "%m".  */
      rc = sexp_build (r_plain, NULL,
                       (ctx.flags & PUBKEY_FLAG_LEGACYRESULT)
                       ? "%m" : "(value %m)",
                       plain);
      break;
    }


 leave:
  xfree (unpad);
  _gcry_mpi_release (plain);
  _gcry_mpi_release (sk.p);
  _gcry_mpi_release (sk.g);
  _gcry_mpi_release (sk.y);
  _gcry_mpi_release (sk.x);
  _gcry_mpi_release (data_a);
  _gcry_mpi_release (data_b);
  sexp_release (l1);
  _gcry_pk_util_free_encoding_ctx (&ctx);
  if (DBG_CIPHER)
    log_debug ("elg_decrypt    => %s\n", gpg_strerror (rc));
  return rc;
}