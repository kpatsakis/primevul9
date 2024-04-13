_gnutls_send_empty_handshake (gnutls_session_t session,
			      gnutls_handshake_description_t type, int again)
{
  opaque data = 0;
  opaque *ptr;

  if (again == 0)
    ptr = &data;
  else
    ptr = NULL;

  return _gnutls_send_handshake (session, ptr, 0, type);
}