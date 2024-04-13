verify_sig (const gnutls_datum_t * tbs,
	    const gnutls_datum_t * signature,
	    gnutls_pk_algorithm_t pk, bigint_t * issuer_params,
	    int issuer_params_size)
{

  switch (pk)
    {
    case GNUTLS_PK_RSA:

      if (_pkcs1_rsa_verify_sig
	  (tbs, signature, issuer_params, issuer_params_size) != 0)
	{
	  gnutls_assert ();
	  return 0;
	}

      return 1;
      break;

    case GNUTLS_PK_DSA:
      if (dsa_verify_sig
	  (tbs, signature, issuer_params, issuer_params_size) != 0)
	{
	  gnutls_assert ();
	  return 0;
	}

      return 1;
      break;
    default:
      gnutls_assert ();
      return GNUTLS_E_INTERNAL_ERROR;

    }
}