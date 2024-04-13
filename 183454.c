_gnutls_read_server_hello (gnutls_session_t session,
			   opaque * data, int datalen)
{
  uint8_t session_id_len = 0;
  int pos = 0;
  int ret = 0;
  gnutls_protocol_t version;
  int len = datalen;

  if (datalen < 38)
    {
      gnutls_assert ();
      return GNUTLS_E_UNEXPECTED_PACKET_LENGTH;
    }

  _gnutls_handshake_log ("HSK[%x]: Server's version: %d.%d\n",
			 session, data[pos], data[pos + 1]);

  DECR_LEN (len, 2);
  version = _gnutls_version_get (data[pos], data[pos + 1]);
  if (_gnutls_version_is_supported (session, version) == 0)
    {
      gnutls_assert ();
      return GNUTLS_E_UNSUPPORTED_VERSION_PACKET;
    }
  else
    {
      _gnutls_set_current_version (session, version);
    }

  pos += 2;

  DECR_LEN (len, TLS_RANDOM_SIZE);
  _gnutls_set_server_random (session, &data[pos]);
  pos += TLS_RANDOM_SIZE;


  /* Read session ID
   */
  DECR_LEN (len, 1);
  session_id_len = data[pos++];

  if (len < session_id_len)
    {
      gnutls_assert ();
      return GNUTLS_E_UNSUPPORTED_VERSION_PACKET;
    }
  DECR_LEN (len, session_id_len);


  /* check if we are resuming and set the appropriate
   * values;
   */
  if (_gnutls_client_check_if_resuming
      (session, &data[pos], session_id_len) == 0)
    return 0;
  pos += session_id_len;


  /* Check if the given cipher suite is supported and copy
   * it to the session.
   */

  DECR_LEN (len, 2);
  ret = _gnutls_client_set_ciphersuite (session, &data[pos]);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }
  pos += 2;



  /* move to compression 
   */
  DECR_LEN (len, 1);

  ret = _gnutls_client_set_comp_method (session, data[pos++]);
  if (ret < 0)
    {
      gnutls_assert ();
      return GNUTLS_E_UNKNOWN_COMPRESSION_ALGORITHM;
    }

  /* Parse extensions.
   */
  if (version >= GNUTLS_TLS1)
    {
      ret = _gnutls_parse_extensions (session, EXTENSION_ANY, &data[pos], len);	/* len is the rest of the parsed length */
      if (ret < 0)
	{
	  gnutls_assert ();
	  return ret;
	}
    }
  return ret;
}