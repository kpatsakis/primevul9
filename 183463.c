_gnutls_send_client_hello (gnutls_session_t session, int again)
{
  opaque *data = NULL;
  int extdatalen;
  int pos = 0;
  int datalen = 0, ret = 0;
  opaque rnd[TLS_RANDOM_SIZE];
  gnutls_protocol_t hver;
  opaque extdata[MAX_EXT_DATA_LENGTH];

  opaque *SessionID =
    session->internals.resumed_security_parameters.session_id;
  uint8_t session_id_len =
    session->internals.resumed_security_parameters.session_id_size;

  if (SessionID == NULL)
    session_id_len = 0;
  else if (session_id_len == 0)
    SessionID = NULL;

  if (again == 0)
    {

      datalen = 2 + (session_id_len + 1) + TLS_RANDOM_SIZE;
      /* 2 for version, (4 for unix time + 28 for random bytes==TLS_RANDOM_SIZE) 
       */

      data = gnutls_malloc (datalen);
      if (data == NULL)
	{
	  gnutls_assert ();
	  return GNUTLS_E_MEMORY_ERROR;
	}

      /* if we are resuming a session then we set the
       * version number to the previously established.
       */
      if (SessionID == NULL)
	hver = _gnutls_version_max (session);
      else
	{			/* we are resuming a session */
	  hver = session->internals.resumed_security_parameters.version;
	}

      if (hver == GNUTLS_VERSION_UNKNOWN || hver == 0)
	{
	  gnutls_assert ();
	  gnutls_free (data);
	  return GNUTLS_E_INTERNAL_ERROR;
	}

      data[pos++] = _gnutls_version_get_major (hver);
      data[pos++] = _gnutls_version_get_minor (hver);

      /* Set the version we advertized as maximum 
       * (RSA uses it).
       */
      _gnutls_set_adv_version (session, hver);

      /* Some old implementations do not interoperate if we send a
       * different version in the record layer.
       * It seems they prefer to read the record's version
       * as the one we actually requested.
       * The proper behaviour is to use the one in the client hello 
       * handshake packet and ignore the one in the packet's record 
       * header.
       */
      _gnutls_set_current_version (session, hver);

      /* In order to know when this session was initiated.
       */
      session->security_parameters.timestamp = time (NULL);

      /* Generate random data 
       */
      _gnutls_tls_create_random (rnd);
      _gnutls_set_client_random (session, rnd);

      memcpy (&data[pos], rnd, TLS_RANDOM_SIZE);
      pos += TLS_RANDOM_SIZE;

      /* Copy the Session ID 
       */
      data[pos++] = session_id_len;

      if (session_id_len > 0)
	{
	  memcpy (&data[pos], SessionID, session_id_len);
	  pos += session_id_len;
	}


      /* Copy the ciphersuites.
       */
      extdatalen =
	_gnutls_copy_ciphersuites (session, extdata, sizeof (extdata));
      if (extdatalen > 0)
	{
	  datalen += extdatalen;
	  data = gnutls_realloc_fast (data, datalen);
	  if (data == NULL)
	    {
	      gnutls_assert ();
	      return GNUTLS_E_MEMORY_ERROR;
	    }

	  memcpy (&data[pos], extdata, extdatalen);
	  pos += extdatalen;

	}
      else
	{
	  if (extdatalen == 0)
	    extdatalen = GNUTLS_E_INTERNAL_ERROR;
	  gnutls_free (data);
	  gnutls_assert ();
	  return extdatalen;
	}


      /* Copy the compression methods.
       */
      extdatalen =
	_gnutls_copy_comp_methods (session, extdata, sizeof (extdata));
      if (extdatalen > 0)
	{
	  datalen += extdatalen;
	  data = gnutls_realloc_fast (data, datalen);
	  if (data == NULL)
	    {
	      gnutls_assert ();
	      return GNUTLS_E_MEMORY_ERROR;
	    }

	  memcpy (&data[pos], extdata, extdatalen);
	  pos += extdatalen;

	}
      else
	{
	  if (extdatalen == 0)
	    extdatalen = GNUTLS_E_INTERNAL_ERROR;
	  gnutls_free (data);
	  gnutls_assert ();
	  return extdatalen;
	}

      /* Generate and copy TLS extensions.
       */
      if (hver >= GNUTLS_TLS1)
	{
	  extdatalen =
	    _gnutls_gen_extensions (session, extdata, sizeof (extdata));

	  if (extdatalen > 0)
	    {
	      datalen += extdatalen;
	      data = gnutls_realloc_fast (data, datalen);
	      if (data == NULL)
		{
		  gnutls_assert ();
		  return GNUTLS_E_MEMORY_ERROR;
		}

	      memcpy (&data[pos], extdata, extdatalen);
	    }
	  else if (extdatalen < 0)
	    {
	      gnutls_assert ();
	      gnutls_free (data);
	      return extdatalen;
	    }
	}
    }

  ret =
    _gnutls_send_handshake (session, data, datalen,
			    GNUTLS_HANDSHAKE_CLIENT_HELLO);
  gnutls_free (data);

  return ret;
}