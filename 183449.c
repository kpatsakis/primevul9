gnutls_rehandshake (gnutls_session_t session)
{
  int ret;

  /* only server sends that handshake packet */
  if (session->security_parameters.entity == GNUTLS_CLIENT)
    return GNUTLS_E_INVALID_REQUEST;

  ret =
    _gnutls_send_empty_handshake (session, GNUTLS_HANDSHAKE_HELLO_REQUEST,
				  AGAIN (STATE50));
  STATE = STATE50;

  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }
  STATE = STATE0;

  return 0;
}