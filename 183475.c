_gnutls_remove_unwanted_ciphersuites (gnutls_session_t session,
				      cipher_suite_st ** cipherSuites,
				      int numCipherSuites,
				      gnutls_pk_algorithm_t requested_pk_algo)
{

  int ret = 0;
  cipher_suite_st *newSuite, cs;
  int newSuiteSize = 0, i;
  gnutls_certificate_credentials_t cert_cred;
  gnutls_kx_algorithm_t kx;
  int server = session->security_parameters.entity == GNUTLS_SERVER ? 1 : 0;
  gnutls_kx_algorithm_t *alg = NULL;
  int alg_size = 0;

  /* if we should use a specific certificate, 
   * we should remove all algorithms that are not supported
   * by that certificate and are on the same authentication
   * method (CERTIFICATE).
   */

  cert_cred =
    (gnutls_certificate_credentials_t) _gnutls_get_cred (session->key,
							 GNUTLS_CRD_CERTIFICATE,
							 NULL);

  /* If there are certificate credentials, find an appropriate certificate
   * or disable them;
   */
  if (session->security_parameters.entity == GNUTLS_SERVER
      && cert_cred != NULL)
    {
      ret = _gnutls_server_select_cert (session, requested_pk_algo);
      if (ret < 0)
	{
	  gnutls_assert ();
	  _gnutls_x509_log("Could not find an appropriate certificate: %s\n", gnutls_strerror(ret));
	  cert_cred = NULL;
	}
    }

  /* get all the key exchange algorithms that are 
   * supported by the X509 certificate parameters.
   */
  if ((ret =
       _gnutls_selected_cert_supported_kx (session, &alg, &alg_size)) < 0)
    {
      gnutls_assert ();
      return ret;
    }

  newSuite = gnutls_malloc (numCipherSuites * sizeof (cipher_suite_st));
  if (newSuite == NULL)
    {
      gnutls_assert ();
      gnutls_free (alg);
      return GNUTLS_E_MEMORY_ERROR;
    }

  /* now removes ciphersuites based on the KX algorithm
   */
  for (i = 0; i < numCipherSuites; i++)
    {
      int delete = 0;

      /* finds the key exchange algorithm in
       * the ciphersuite
       */
      kx = _gnutls_cipher_suite_get_kx_algo (&(*cipherSuites)[i]);

      /* if it is defined but had no credentials 
       */
      if (_gnutls_get_kx_cred (session, kx, NULL) == NULL)
	{
	  delete = 1;
	}
      else
	{
	  delete = 0;

	  if (server)
	    delete = check_server_params (session, kx, alg, alg_size);
	}

      /* These two SRP kx's are marked to require a CRD_CERTIFICATE,
	 (see cred_mappings in gnutls_algorithms.c), but it also
	 requires a SRP credential.  Don't use SRP kx unless we have a
	 SRP credential too.  */
      if (kx == GNUTLS_KX_SRP_RSA || kx == GNUTLS_KX_SRP_DSS)
	{
	  if (!_gnutls_get_cred (session->key, GNUTLS_CRD_SRP, NULL))
	    delete = 1;
	}

      memcpy (&cs.suite, &(*cipherSuites)[i].suite, 2);

      if (delete == 0)
	{

	  _gnutls_handshake_log ("HSK[%x]: Keeping ciphersuite: %s\n",
				 session,
				 _gnutls_cipher_suite_get_name (&cs));

	  memcpy (newSuite[newSuiteSize].suite, (*cipherSuites)[i].suite, 2);
	  newSuiteSize++;
	}
      else
	{
	  _gnutls_handshake_log ("HSK[%x]: Removing ciphersuite: %s\n",
				 session,
				 _gnutls_cipher_suite_get_name (&cs));

	}
    }

  gnutls_free (alg);
  gnutls_free (*cipherSuites);
  *cipherSuites = newSuite;

  ret = newSuiteSize;

  return ret;

}