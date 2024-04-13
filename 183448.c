_gnutls_handshake_hash_add_sent (gnutls_session_t session,
				 gnutls_handshake_description_t type,
				 opaque * dataptr, uint32_t datalen)
{
  int ret;

  if ((ret = _gnutls_handshake_hash_pending (session)) < 0)
    {
      gnutls_assert ();
      return ret;
    }

  if (type != GNUTLS_HANDSHAKE_HELLO_REQUEST)
    {
      _gnutls_hash (&session->internals.handshake_mac_handle_sha, dataptr,
		    datalen);
      _gnutls_hash (&session->internals.handshake_mac_handle_md5, dataptr,
		    datalen);
    }

  return 0;
}