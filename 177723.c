elg_check_secret_key (gcry_sexp_t keyparms)
{
  gcry_err_code_t rc;
  ELG_secret_key sk = {NULL, NULL, NULL, NULL};

  rc = sexp_extract_param (keyparms, NULL, "pgyx",
                           &sk.p, &sk.g, &sk.y, &sk.x,
                           NULL);
  if (rc)
    goto leave;

  if (!check_secret_key (&sk))
    rc = GPG_ERR_BAD_SECKEY;

 leave:
  _gcry_mpi_release (sk.p);
  _gcry_mpi_release (sk.g);
  _gcry_mpi_release (sk.y);
  _gcry_mpi_release (sk.x);
  if (DBG_CIPHER)
    log_debug ("elg_testkey    => %s\n", gpg_strerror (rc));
  return rc;
}