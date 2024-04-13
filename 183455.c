_gnutls_handshake_hash_buffers_clear (gnutls_session_t session)
{
  _gnutls_hash_deinit (&session->internals.handshake_mac_handle_md5, NULL);
  _gnutls_hash_deinit (&session->internals.handshake_mac_handle_sha, NULL);
  _gnutls_handshake_buffer_clear (session);
}