_gnutls_handshake_common (gnutls_session_t session)
{
  int ret = 0;

  /* send and recv the change cipher spec and finished messages */
  if ((session->internals.resumed == RESUME_TRUE
       && session->security_parameters.entity == GNUTLS_CLIENT)
      || (session->internals.resumed == RESUME_FALSE
	  && session->security_parameters.entity == GNUTLS_SERVER))
    {
      /* if we are a client resuming - or we are a server not resuming */

      ret = _gnutls_recv_handshake_final (session, TRUE);
      IMED_RET ("recv handshake final", ret);

      ret = _gnutls_send_handshake_final (session, FALSE);
      IMED_RET ("send handshake final", ret);
    }
  else
    {				/* if we are a client not resuming - or we are a server resuming */

      ret = _gnutls_send_handshake_final (session, TRUE);
      IMED_RET ("send handshake final 2", ret);

      ret = _gnutls_recv_handshake_final (session, FALSE);
      IMED_RET ("recv handshake final 2", ret);
    }

  if (session->security_parameters.entity == GNUTLS_SERVER)
    {
      /* in order to support session resuming */
      _gnutls_server_register_current_session (session);
    }

  /* clear handshake buffer */
  _gnutls_handshake_hash_buffers_clear (session);
  return ret;

}