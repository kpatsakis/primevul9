_gnutls_recv_hello_request (gnutls_session_t session, void *data,
			    uint32_t data_size)
{
  uint8_t type;

  if (session->security_parameters.entity == GNUTLS_SERVER)
    {
      gnutls_assert ();
      return GNUTLS_E_UNEXPECTED_PACKET;
    }
  if (data_size < 1)
    {
      gnutls_assert ();
      return GNUTLS_E_UNEXPECTED_PACKET_LENGTH;
    }
  type = ((uint8_t *) data)[0];
  if (type == GNUTLS_HANDSHAKE_HELLO_REQUEST)
    return GNUTLS_E_REHANDSHAKE;
  else
    {
      gnutls_assert ();
      return GNUTLS_E_UNEXPECTED_PACKET;
    }
}