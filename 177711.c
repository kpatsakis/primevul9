elg_verify (gcry_sexp_t s_sig, gcry_sexp_t s_data, gcry_sexp_t s_keyparms)
{
  gcry_err_code_t rc;
  struct pk_encoding_ctx ctx;
  gcry_sexp_t l1 = NULL;
  gcry_mpi_t sig_r = NULL;
  gcry_mpi_t sig_s = NULL;
  gcry_mpi_t data = NULL;
  ELG_public_key pk = { NULL, NULL, NULL };

  _gcry_pk_util_init_encoding_ctx (&ctx, PUBKEY_OP_VERIFY,
                                   elg_get_nbits (s_keyparms));

  /* Extract the data.  */
  rc = _gcry_pk_util_data_to_mpi (s_data, &data, &ctx);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    log_mpidump ("elg_verify data", data);
  if (mpi_is_opaque (data))
    {
      rc = GPG_ERR_INV_DATA;
      goto leave;
    }

  /* Extract the signature value.  */
  rc = _gcry_pk_util_preparse_sigval (s_sig, elg_names, &l1, NULL);
  if (rc)
    goto leave;
  rc = sexp_extract_param (l1, NULL, "rs", &sig_r, &sig_s, NULL);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    {
      log_mpidump ("elg_verify  s_r", sig_r);
      log_mpidump ("elg_verify  s_s", sig_s);
    }

  /* Extract the key.  */
  rc = sexp_extract_param (s_keyparms, NULL, "pgy",
                                 &pk.p, &pk.g, &pk.y, NULL);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    {
      log_mpidump ("elg_verify    p", pk.p);
      log_mpidump ("elg_verify    g", pk.g);
      log_mpidump ("elg_verify    y", pk.y);
    }

  /* Verify the signature.  */
  if (!verify (sig_r, sig_s, data, &pk))
    rc = GPG_ERR_BAD_SIGNATURE;

 leave:
  _gcry_mpi_release (pk.p);
  _gcry_mpi_release (pk.g);
  _gcry_mpi_release (pk.y);
  _gcry_mpi_release (data);
  _gcry_mpi_release (sig_r);
  _gcry_mpi_release (sig_s);
  sexp_release (l1);
  _gcry_pk_util_free_encoding_ctx (&ctx);
  if (DBG_CIPHER)
    log_debug ("elg_verify    => %s\n", rc?gpg_strerror (rc):"Good");
  return rc;
}