check_server_params (gnutls_session_t session,
		     gnutls_kx_algorithm_t kx,
		     gnutls_kx_algorithm_t * alg, int alg_size)
{
  int cred_type;
  gnutls_dh_params_t dh_params = NULL;
  gnutls_rsa_params_t rsa_params = NULL;
  int j;

  cred_type = _gnutls_map_kx_get_cred (kx, 1);

  /* Read the Diffie Hellman parameters, if any.
   */
  if (cred_type == GNUTLS_CRD_CERTIFICATE)
    {
      int delete;
      gnutls_certificate_credentials_t x509_cred =
	(gnutls_certificate_credentials_t) _gnutls_get_cred (session->key,
							     cred_type, NULL);

      if (x509_cred != NULL)
	{
	  dh_params =
	    _gnutls_get_dh_params (x509_cred->dh_params,
				   x509_cred->params_func, session);
	  rsa_params =
	    _gnutls_certificate_get_rsa_params (x509_cred->rsa_params,
						x509_cred->params_func,
						session);
	}

      /* Check also if the certificate supports the
       * KX method.
       */
      delete = 1;
      for (j = 0; j < alg_size; j++)
	{
	  if (alg[j] == kx)
	    {
	      delete = 0;
	      break;
	    }
	}

      if (delete == 1)
	return 1;

#ifdef ENABLE_ANON
    }
  else if (cred_type == GNUTLS_CRD_ANON)
    {
      gnutls_anon_server_credentials_t anon_cred =
	(gnutls_anon_server_credentials_t) _gnutls_get_cred (session->key,
							     cred_type, NULL);

      if (anon_cred != NULL)
	{
	  dh_params =
	    _gnutls_get_dh_params (anon_cred->dh_params,
				   anon_cred->params_func, session);
	}
#endif
#ifdef ENABLE_PSK
    }
  else if (cred_type == GNUTLS_CRD_PSK)
    {
      gnutls_psk_server_credentials_t psk_cred =
	(gnutls_psk_server_credentials_t) _gnutls_get_cred (session->key,
							    cred_type, NULL);

      if (psk_cred != NULL)
	{
	  dh_params =
	    _gnutls_get_dh_params (psk_cred->dh_params, psk_cred->params_func,
				   session);
	}
#endif
    }
  else
    return 0;			/* no need for params */


  /* If the key exchange method needs RSA or DH params,
   * but they are not set then remove it.
   */
  if (_gnutls_kx_needs_rsa_params (kx) != 0)
    {
      /* needs rsa params. */
      if (_gnutls_rsa_params_to_mpi (rsa_params) == NULL)
	{
	  gnutls_assert ();
	  return 1;
	}
    }

  if (_gnutls_kx_needs_dh_params (kx) != 0)
    {
      /* needs DH params. */
      if (_gnutls_dh_params_to_mpi (dh_params) == NULL)
	{
	  gnutls_assert ();
	  return 1;
	}
    }

  return 0;
}