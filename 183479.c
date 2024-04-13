_gnutls_finished (gnutls_session_t session, int type, void *ret)
{
  const int siz = TLS_MSG_LEN;
  opaque concat[36];
  size_t len;
  const char *mesg;
  digest_hd_st td_md5;
  digest_hd_st td_sha;
  gnutls_protocol_t ver = gnutls_protocol_get_version (session);
  int rc;

  if (ver < GNUTLS_TLS1_2)
    {
      rc = _gnutls_hash_copy (&td_md5, &session->internals.handshake_mac_handle_md5);
      if (rc < 0)
	{
	  gnutls_assert ();
	  return rc;
	}
    }

  rc = _gnutls_hash_copy (&td_sha, &session->internals.handshake_mac_handle_sha);
  if (rc < 0)
    {
      gnutls_assert ();
      _gnutls_hash_deinit (&td_md5, NULL);
      return rc;
    }

  if (ver < GNUTLS_TLS1_2)
    {
      _gnutls_hash_deinit (&td_md5, concat);
      _gnutls_hash_deinit (&td_sha, &concat[16]);
      len = 20 + 16;
    }
  else
    {
      _gnutls_hash_deinit (&td_sha, concat);
      len = 20;
    }

  if (type == GNUTLS_SERVER)
    {
      mesg = SERVER_MSG;
    }
  else
    {
      mesg = CLIENT_MSG;
    }

  return _gnutls_PRF (session, session->security_parameters.master_secret,
		      TLS_MASTER_SIZE, mesg, siz, concat, len, 12, ret);
}