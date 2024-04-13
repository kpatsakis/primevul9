_gnutls_server_select_suite (gnutls_session_t session, opaque * data,
			     int datalen)
{
  int x, i, j;
  cipher_suite_st *ciphers, cs;
  int retval, err;
  gnutls_pk_algorithm_t pk_algo;	/* will hold the pk algorithms
					 * supported by the peer.
					 */

  pk_algo = _gnutls_server_find_pk_algos_in_ciphersuites (data, datalen);

  x = _gnutls_supported_ciphersuites (session, &ciphers);
  if (x < 0)
    {				/* the case x==0 is handled within the function. */
      gnutls_assert ();
      return x;
    }

  /* Here we remove any ciphersuite that does not conform
   * the certificate requested, or to the
   * authentication requested (e.g. SRP).
   */
  x = _gnutls_remove_unwanted_ciphersuites (session, &ciphers, x, pk_algo);
  if (x <= 0)
    {
      gnutls_assert ();
      gnutls_free (ciphers);
      if (x < 0)
	return x;
      else
	return GNUTLS_E_UNKNOWN_CIPHER_SUITE;
    }

  /* Data length should be zero mod 2 since
   * every ciphersuite is 2 bytes. (this check is needed
   * see below).
   */
  if (datalen % 2 != 0)
    {
      gnutls_assert ();
      return GNUTLS_E_UNEXPECTED_PACKET_LENGTH;
    }
#ifdef HANDSHAKE_DEBUG

  _gnutls_handshake_log ("HSK[%x]: Requested cipher suites: \n", session);
  for (j = 0; j < datalen; j += 2)
    {
      memcpy (&cs.suite, &data[j], 2);
      _gnutls_handshake_log ("\t%s\n", _gnutls_cipher_suite_get_name (&cs));
    }
  _gnutls_handshake_log ("HSK[%x]: Supported cipher suites: \n", session);
  for (j = 0; j < x; j++)
    _gnutls_handshake_log ("\t%s\n",
			   _gnutls_cipher_suite_get_name (&ciphers[j]));
#endif
  memset (session->security_parameters.current_cipher_suite.suite, '\0', 2);

  retval = GNUTLS_E_UNKNOWN_CIPHER_SUITE;

  for (j = 0; j < datalen; j += 2)
    {
      for (i = 0; i < x; i++)
	{
	  if (memcmp (ciphers[i].suite, &data[j], 2) == 0)
	    {
	      memcpy (&cs.suite, &data[j], 2);

	      _gnutls_handshake_log
		("HSK[%x]: Selected cipher suite: %s\n", session,
		 _gnutls_cipher_suite_get_name (&cs));
	      memcpy (session->security_parameters.current_cipher_suite.
		      suite, ciphers[i].suite, 2);
	      retval = 0;
	      goto finish;
	    }
	}
    }

finish:
  gnutls_free (ciphers);

  if (retval != 0)
    {
      gnutls_assert ();
      return retval;
    }

  /* check if the credentials (username, public key etc.) are ok
   */
  if (_gnutls_get_kx_cred
      (session,
       _gnutls_cipher_suite_get_kx_algo (&session->security_parameters.
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