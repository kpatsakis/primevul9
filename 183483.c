_gnutls_client_set_ciphersuite (gnutls_session_t session, opaque suite[2])
{
  uint8_t z;
  cipher_suite_st *cipher_suites;
  int cipher_suite_num;
  int i, err;

  z = 1;
  cipher_suite_num = _gnutls_supported_ciphersuites (session, &cipher_suites);
  if (cipher_suite_num < 0)
    {
      gnutls_assert ();
      return cipher_suite_num;
    }

  for (i = 0; i < cipher_suite_num; i++)
    {
      if (memcmp (&cipher_suites[i], suite, 2) == 0)
	{
	  z = 0;
	  break;
	}
    }

  gnutls_free (cipher_suites);

  if (z != 0)
    {
      gnutls_assert ();
      return GNUTLS_E_UNKNOWN_CIPHER_SUITE;
    }

  memcpy (session->security_parameters.current_cipher_suite.suite, suite, 2);

  _gnutls_handshake_log ("HSK[%x]: Selected cipher suite: %s\n", session,
			 _gnutls_cipher_suite_get_name (&session->
							security_parameters.
							current_cipher_suite));


  /* check if the credentials (username, public key etc.) are ok.
   * Actually checks if they exist.
   */
  if (_gnutls_get_kx_cred
      (session, _gnutls_cipher_suite_get_kx_algo (&session->
						  security_parameters.
						  current_cipher_suite),
       &err) == NULL && err != 0)
    {
      gnutls_assert ();
      return GNUTLS_E_INSUFFICIENT_CREDENTIALS;
    }


  /* set the mod_auth_st to the appropriate struct
   * according to the KX algorithm. This is needed since all the
   * handshake functions are read from there;
   */
  session->internals.auth_struct =
    _gnutls_kx_auth_struct (_gnutls_cipher_suite_get_kx_algo
			    (&session->security_parameters.
			     current_cipher_suite));

  if (session->internals.auth_struct == NULL)
    {

      _gnutls_handshake_log
	("HSK[%x]: Cannot find the appropriate handler for the KX algorithm\n",
	 session);
      gnutls_assert ();
      return GNUTLS_E_INTERNAL_ERROR;
    }


  return 0;
}