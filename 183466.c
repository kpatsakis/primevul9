gnutls_handshake (gnutls_session_t session)
{
  int ret;

  if ((ret = _gnutls_handshake_hash_init (session)) < 0)
    {
      gnutls_assert ();
      return ret;
    }

  if (session->security_parameters.entity == GNUTLS_CLIENT)
    {
      ret = _gnutls_handshake_client (session);
    }
  else
    {
      ret = _gnutls_handshake_server (session);
    }
  if (ret < 0)
    {
      /* In the case of a rehandshake abort
       * we should reset the handshake's internal state.
       */
      if (_gnutls_abort_handshake (session, ret) == 0)
	STATE = STATE0;

      return ret;
    }

  ret = _gnutls_handshake_common (session);

  if (ret < 0)
    {
      if (_gnutls_abort_handshake (session, ret) == 0)
	STATE = STATE0;

      return ret;
    }

  STATE = STATE0;

  _gnutls_handshake_io_buffer_clear (session);
  _gnutls_handshake_internal_state_clear (session);

  return 0;
}