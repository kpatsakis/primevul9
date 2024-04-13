gnutls_handshake_get_last_out (gnutls_session_t session)
{
  return session->internals.last_handshake_out;
}