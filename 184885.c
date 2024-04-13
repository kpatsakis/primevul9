_gnutls_x509_verify_signature (const gnutls_datum_t * tbs,
			       const gnutls_datum_t * signature,
			       gnutls_x509_crt_t issuer)
{
  bigint_t issuer_params[MAX_PUBLIC_PARAMS_SIZE];
  int ret, issuer_params_size, i;

  /* Read the MPI parameters from the issuer's certificate.
   */
  issuer_params_size = MAX_PUBLIC_PARAMS_SIZE;
  ret =
    _gnutls_x509_crt_get_mpis (issuer, issuer_params, &issuer_params_size);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  ret =
    verify_sig (tbs, signature,
		gnutls_x509_crt_get_pk_algorithm (issuer, NULL),
		issuer_params, issuer_params_size);
  if (ret < 0)
    {
      gnutls_assert ();
    }

  /* release all allocated MPIs
   */
  for (i = 0; i < issuer_params_size; i++)
    {
      _gnutls_mpi_release (&issuer_params[i]);
    }

  return ret;
}