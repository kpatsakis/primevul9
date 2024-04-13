gnutls_handshake_get_last_in (gnutls_session_t session)
{
  return session->internals.last_handshake_in;
}