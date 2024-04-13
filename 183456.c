_gnutls_handshake_server (gnutls_session_t session)
{
  int ret = 0;

  switch (STATE)
    {
    case STATE0:
    case STATE1:
      ret =
	_gnutls_recv_handshake (session, NULL, NULL,
				GNUTLS_HANDSHAKE_CLIENT_HELLO,
				MANDATORY_PACKET);
      STATE = STATE1;
      IMED_RET ("recv hello", ret);

    case STATE2:
      ret = _gnutls_send_hello (session, AGAIN (STATE2));
      STATE = STATE2;
      IMED_RET ("send hello", ret);

    case STATE70:
      if (session->security_parameters.extensions.do_send_supplemental)
	{
	  ret = _gnutls_send_supplemental (session, AGAIN (STATE70));
	  STATE = STATE70;
	  IMED_RET ("send supplemental data", ret);
	}

      /* SEND CERTIFICATE + KEYEXCHANGE + CERTIFICATE_REQUEST */
    case STATE3:
      /* NOTE: these should not be send if we are resuming */

      if (session->internals.resumed == RESUME_FALSE)
	ret = _gnutls_send_server_certificate (session, AGAIN (STATE3));
      STATE = STATE3;
      IMED_RET ("send server certificate", ret);

    case STATE4:
      /* send server key exchange (A) */
      if (session->internals.resumed == RESUME_FALSE)
	ret = _gnutls_send_server_kx_message (session, AGAIN (STATE4));
      STATE = STATE4;
      IMED_RET ("send server kx", ret);

    case STATE5:
      /* Send certificate request - if requested to */
      if (session->internals.resumed == RESUME_FALSE)
	ret =
	  _gnutls_send_server_certificate_request (session, AGAIN (STATE5));
      STATE = STATE5;
      IMED_RET ("send server cert request", ret);

    case STATE6:
      /* send the server hello done */
      if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
	ret =
	  _gnutls_send_empty_handshake (session,
					GNUTLS_HANDSHAKE_SERVER_HELLO_DONE,
					AGAIN (STATE6));
      STATE = STATE6;
      IMED_RET ("send server hello done", ret);

    case STATE71:
      if (session->security_parameters.extensions.do_recv_supplemental)
	{
	  ret = _gnutls_recv_supplemental (session);
	  STATE = STATE71;
	  IMED_RET ("recv client supplemental", ret);
	}

      /* RECV CERTIFICATE + KEYEXCHANGE + CERTIFICATE_VERIFY */
    case STATE7:
      /* receive the client certificate message */
      if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
	ret = _gnutls_recv_client_certificate (session);
      STATE = STATE7;
      IMED_RET ("recv client certificate", ret);

    case STATE8:
      /* receive the client key exchange message */
      if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
	ret = _gnutls_recv_client_kx_message (session);
      STATE = STATE8;
      IMED_RET ("recv client kx", ret);

    case STATE9:
      /* receive the client certificate verify message */
      if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
	ret = _gnutls_recv_client_certificate_verify_message (session);
      STATE = STATE9;
      IMED_RET ("recv client certificate verify", ret);

      STATE = STATE0;		/* finished thus clear session */
    default:
      break;
    }

  return 0;
}