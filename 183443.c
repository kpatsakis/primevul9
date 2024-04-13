_gnutls_read_client_hello (gnutls_session_t session, opaque * data,
			   int datalen)
{
  uint8_t session_id_len;
  int pos = 0, ret;
  uint16_t suite_size, comp_size;
  gnutls_protocol_t adv_version;
  int neg_version;
  int len = datalen;
  opaque rnd[TLS_RANDOM_SIZE], *suite_ptr, *comp_ptr;

  if (session->internals.v2_hello != 0)
    {				/* version 2.0 */
      return _gnutls_read_client_hello_v2 (session, data, datalen);
    }
  DECR_LEN (len, 2);

  _gnutls_handshake_log ("HSK[%x]: Client's version: %d.%d\n", session,
			 data[pos], data[pos + 1]);

  adv_version = _gnutls_version_get (data[pos], data[pos + 1]);
  set_adv_version (session, data[pos], data[pos + 1]);
  pos += 2;

  neg_version = _gnutls_negotiate_version( session, adv_version);
  if (neg_version < 0)
    {
      gnutls_assert();
      return neg_version;
    }

  /* Read client random value.
   */
  DECR_LEN (len, TLS_RANDOM_SIZE);
  _gnutls_set_client_random (session, &data[pos]);
  pos += TLS_RANDOM_SIZE;

  _gnutls_tls_create_random (rnd);
  _gnutls_set_server_random (session, rnd);

  session->security_parameters.timestamp = time (NULL);

  DECR_LEN (len, 1);
  session_id_len = data[pos++];

  /* RESUME SESSION 
   */
  if (session_id_len > TLS_MAX_SESSION_ID_SIZE)
    {
      gnutls_assert ();
      return GNUTLS_E_UNEXPECTED_PACKET_LENGTH;
    }
  DECR_LEN (len, session_id_len);
  
  ret = _gnutls_server_restore_session (session, &data[pos], session_id_len);
  pos += session_id_len;

  if (ret == 0)
    {				/* resumed! */
      resume_copy_required_values (session);
      session->internals.resumed = RESUME_TRUE;
      return _gnutls_user_hello_func( session, adv_version);
    }
  else
    {
      _gnutls_generate_session_id (session->security_parameters.
				   session_id,
				   &session->security_parameters.
				   session_id_size);

      session->internals.resumed = RESUME_FALSE;
    }

  /* Remember ciphersuites for later
   */
  DECR_LEN (len, 2);
  suite_size = _gnutls_read_uint16 (&data[pos]);
  pos += 2;

  DECR_LEN (len, suite_size);
  suite_ptr = &data[pos];
  pos += suite_size;

  /* Point to the compression methods
   */
  DECR_LEN (len, 1);
  comp_size = data[pos++];		/* z is the number of compression methods */

  DECR_LEN (len, comp_size);
  comp_ptr = &data[pos];
  pos += comp_size;

  /* Parse the extensions (if any)
   */
  if (neg_version >= GNUTLS_TLS1)
    {
      ret = _gnutls_parse_extensions (session, EXTENSION_APPLICATION, &data[pos], len);	/* len is the rest of the parsed length */
      if (ret < 0)
	{
	  gnutls_assert ();
	  return ret;
	}
    }

  ret = _gnutls_user_hello_func( session, adv_version);
  if (ret < 0) 
    {
      gnutls_assert();
      return ret;
    }
  
  if (neg_version >= GNUTLS_TLS1)
    {
      ret = _gnutls_parse_extensions (session, EXTENSION_TLS, &data[pos], len);	/* len is the rest of the parsed length */
      if (ret < 0)
	{
	  gnutls_assert ();
	  return ret;
	}
    }

  /* select an appropriate cipher suite
   */
  ret = _gnutls_server_select_suite (session, suite_ptr, suite_size);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  /* select appropriate compression method */
  ret = _gnutls_server_select_comp_method (session, comp_ptr, comp_size);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  return 0;
}