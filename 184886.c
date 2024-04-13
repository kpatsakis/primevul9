_gnutls_x509_privkey_verify_signature (const gnutls_datum_t * tbs,
				       const gnutls_datum_t * signature,
				       gnutls_x509_privkey_t issuer)
{
  int ret;

  ret = verify_sig (tbs, signature, issuer->pk_algorithm,
		    issuer->params, issuer->params_size);
  if (ret < 0)
    {
      gnutls_assert ();
    }

  return ret;
}