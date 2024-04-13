_gnutls_send_server_hello (gnutls_session_t session, int again)
{
  opaque *data = NULL;
  opaque extdata[MAX_EXT_DATA_LENGTH];
  int extdatalen;
  int pos = 0;
  int datalen, ret = 0;
  uint8_t comp;
  opaque *SessionID = session->security_parameters.session_id;
  uint8_t session_id_len = session->security_parameters.session_id_size;
  opaque buf[2 * TLS_MAX_SESSION_ID_SIZE + 1];

  if (SessionID == NULL)
    session_id_len = 0;

  datalen = 0;

#ifdef ENABLE_SRP
  if (IS_SRP_KX
      (_gnutls_cipher_suite_get_kx_algo
       (&session->security_parameters.current_cipher_suite)))
    {
      /* While resuming we cannot check the username extension since it is
       * not available at this point. It will be copied on connection
       * state activation.
       */
      if (session->internals.resumed == RESUME_FALSE &&
	  session->security_parameters.extensions.srp_username[0] == 0)
	{
	  /* The peer didn't send a valid SRP extension with the
	   * SRP username. The draft requires that we send a fatal
	   * alert and abort.
	   */
	  gnutls_assert ();
	  ret = gnutls_alert_send (session, GNUTLS_AL_FATAL,
				   GNUTLS_A_UNKNOWN_PSK_IDENTITY);
	  if (ret < 0)
	    {
	      gnutls_assert ();
	      return ret;
	    }

	  return GNUTLS_E_ILLEGAL_SRP_USERNAME;
	}
    }
#endif

  if (again == 0)
    {
      datalen = 2 + session_id_len + 1 + TLS_RANDOM_SIZE + 3;
      extdatalen =
	_gnutls_gen_extensions (session, extdata, sizeof (extdata));

      if (extdatalen < 0)
	{
	  gnutls_assert ();
	  return extdatalen;
	}

      data = gnutls_malloc (datalen + extdatalen);
      if (data == NULL)
	{
	  gnutls_assert ();
	  return GNUTLS_E_MEMORY_ERROR;
	}

      data[pos++] =
	_gnutls_version_get_major (session->security_parameters.version);
      data[pos++] =
	_gnutls_version_get_minor (session->security_parameters.version);

      memcpy (&data[pos],
	      session->security_parameters.server_random, TLS_RANDOM_SIZE);
      pos += TLS_RANDOM_SIZE;

      data[pos++] = session_id_len;
      if (session_id_len > 0)
	{
	  memcpy (&data[pos], SessionID, session_id_len);
	}
      pos += session_id_len;

      _gnutls_handshake_log ("HSK[%x]: SessionID: %s\n", session,
			     _gnutls_bin2hex (SessionID, session_id_len,
					      buf, sizeof (buf)));

      memcpy (&data[pos],
	      session->security_parameters.current_cipher_suite.suite, 2);
      pos += 2;

      comp =
	(uint8_t) _gnutls_compression_get_num (session->
					       internals.compression_method);
      data[pos++] = comp;


      if (extdatalen > 0)
	{
	  datalen += extdatalen;

	  memcpy (&data[pos], extdata, extdatalen);
	}
    }

  ret =
    _gnutls_send_handshake (session, data, datalen,
			    GNUTLS_HANDSHAKE_SERVER_HELLO);
  gnutls_free (data);

  return ret;
}