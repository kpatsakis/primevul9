_gnutls_handshake_hash_init (gnutls_session_t session)
{

  if (session->internals.handshake_mac_handle_init == 0)
    {
      int ret =
	_gnutls_hash_init (&session->internals.handshake_mac_handle_md5, GNUTLS_MAC_MD5);

      if (ret < 0)
	{
	  gnutls_assert ();
	  return ret;
	}

      ret = _gnutls_hash_init(&session->internals.handshake_mac_handle_sha, GNUTLS_MAC_SHA1);
      if (ret < 0)
	{
	  gnutls_assert ();
	  return GNUTLS_E_MEMORY_ERROR;
	}
	
      session->internals.handshake_mac_handle_init = 1;
    }

  return 0;
}