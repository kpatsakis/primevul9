_gnutls_handshake_hash_pending (gnutls_session_t session)
{
  size_t siz;
  int ret;
  opaque *data;

  if (session->internals.handshake_mac_handle_init == 0)
    {
      gnutls_assert ();
      return GNUTLS_E_INTERNAL_ERROR;
    }

  /* We check if there are pending data to hash.
   */
  if ((ret = _gnutls_handshake_buffer_get_ptr (session, &data, &siz)) < 0)
    {
      gnutls_assert ();
      return ret;
    }

  if (siz > 0)
    {
      _gnutls_hash (&session->internals.handshake_mac_handle_sha, data, siz);
      _gnutls_hash (&session->internals.handshake_mac_handle_md5, data, siz);
    }

  _gnutls_handshake_buffer_empty (session);

  return 0;
}