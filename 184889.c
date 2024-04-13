_gnutls_verify_crl2 (gnutls_x509_crl_t crl,
		     const gnutls_x509_crt_t * trusted_cas,
		     int tcas_size, unsigned int flags, unsigned int *output)
{
/* CRL is ignored for now */
  gnutls_datum_t crl_signed_data = { NULL, 0 };
  gnutls_datum_t crl_signature = { NULL, 0 };
  gnutls_x509_crt_t issuer;
  int ret, result;

  if (output)
    *output = 0;

  if (tcas_size >= 1)
    issuer = find_crl_issuer (crl, trusted_cas, tcas_size);
  else
    {
      gnutls_assert ();
      if (output)
	*output |= GNUTLS_CERT_SIGNER_NOT_FOUND | GNUTLS_CERT_INVALID;
      return 0;
    }

  /* issuer is not in trusted certificate
   * authorities.
   */
  if (issuer == NULL)
    {
      gnutls_assert ();
      if (output)
	*output |= GNUTLS_CERT_SIGNER_NOT_FOUND | GNUTLS_CERT_INVALID;
      return 0;
    }

  if (!(flags & GNUTLS_VERIFY_DISABLE_CA_SIGN))
    {
      if (gnutls_x509_crt_get_ca_status (issuer, NULL) != 1)
	{
	  gnutls_assert ();
	  if (output)
	    *output |= GNUTLS_CERT_SIGNER_NOT_CA | GNUTLS_CERT_INVALID;
	  return 0;
	}
    }

  result =
    _gnutls_x509_get_signed_data (crl->crl, "tbsCertList", &crl_signed_data);
  if (result < 0)
    {
      gnutls_assert ();
      goto cleanup;
    }

  result = _gnutls_x509_get_signature (crl->crl, "signature", &crl_signature);
  if (result < 0)
    {
      gnutls_assert ();
      goto cleanup;
    }

  ret =
    _gnutls_x509_verify_signature (&crl_signed_data, &crl_signature, issuer);
  if (ret < 0)
    {
      gnutls_assert ();
    }
  else if (ret == 0)
    {
      gnutls_assert ();
      /* error. ignore it */
      if (output)
	*output |= GNUTLS_CERT_INVALID;
      ret = 0;
    }

  {
    int sigalg;

    sigalg = gnutls_x509_crl_get_signature_algorithm (crl);

    if (((sigalg == GNUTLS_SIGN_RSA_MD2) &&
	 !(flags & GNUTLS_VERIFY_ALLOW_SIGN_RSA_MD2)) ||
	((sigalg == GNUTLS_SIGN_RSA_MD5) &&
	 !(flags & GNUTLS_VERIFY_ALLOW_SIGN_RSA_MD5)))
      {
	if (output)
	  *output |= GNUTLS_CERT_INSECURE_ALGORITHM | GNUTLS_CERT_INVALID;
	ret = 0;
      }
  }

  result = ret;

cleanup:
  _gnutls_free_datum (&crl_signed_data);
  _gnutls_free_datum (&crl_signature);

  return result;
}