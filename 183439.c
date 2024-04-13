_gnutls_ssl3_finished (gnutls_session_t session, int type, opaque * ret)
{
  const int siz = SSL_MSG_LEN;
  digest_hd_st td_md5;
  digest_hd_st td_sha;
  const char *mesg;
  int rc;

  rc = _gnutls_hash_copy (&td_md5, &session->internals.handshake_mac_handle_md5);
  if (rc < 0)
    {
      gnutls_assert ();
      return rc;
    }

  rc = _gnutls_hash_copy (&td_sha, &session->internals.handshake_mac_handle_sha);
  if (rc < 0)
    {
      gnutls_assert ();
      _gnutls_hash_deinit (&td_md5, NULL);
      return rc;
    }

  if (type == GNUTLS_SERVER)
    {
      mesg = SSL3_SERVER_MSG;
    }
  else
    {
      mesg = SSL3_CLIENT_MSG;
    }

  _gnutls_hash (&td_md5, mesg, siz);
  _gnutls_hash (&td_sha, mesg, siz);

  _gnutls_mac_deinit_ssl3_handshake (&td_md5, ret,
				     session->security_parameters.
				     master_secret, TLS_MASTER_SIZE);
  _gnutls_mac_deinit_ssl3_handshake (&td_sha, &ret[16],
				     session->security_parameters.
				     master_secret, TLS_MASTER_SIZE);

  return 0;
}