_gnutls_handshake_client (gnutls_session_t session)
{
  int ret = 0;

#ifdef HANDSHAKE_DEBUG
  char buf[64];

  if (session->internals.resumed_security_parameters.session_id_size > 0)
    _gnutls_handshake_log ("HSK[%x]: Ask to resume: %s\n", session,
			   _gnutls_bin2hex (session->internals.
					    resumed_security_parameters.
					    session_id,
					    session->internals.
					    resumed_security_parameters.
					    session_id_size, buf,
					    sizeof (buf)));
#endif

  switch (STATE)
    {
    case STATE0:
    case STATE1:
      ret = _gnutls_send_hello (session, AGAIN (STATE1));
      STATE = STATE1;
      IMED_RET ("send hello", ret);

    case STATE2:
      /* receive the server hello */
      ret =
	_gnutls_recv_handshake (session, NULL, NULL,
				GNUTLS_HANDSHAKE_SERVER_HELLO,
				MANDATORY_PACKET);
      STATE = STATE2;
      IMED_RET ("recv hello", ret);

    case STATE70:
      if (session->security_parameters.extensions.do_recv_supplemental)
	{
	  ret = _gnutls_recv_supplemental (session);
	  STATE = STATE70;
	  IMED_RET ("recv supplemental", ret);
	}

    case STATE3:
      /* RECV CERTIFICATE */
      if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
	ret = _gnutls_recv_server_certificate (session);
      STATE = STATE3;
      IMED_RET ("recv server certificate", ret);

    case STATE4:
      /* receive the server key exchange */
      if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
	ret = _gnutls_recv_server_kx_message (session);
      STATE = STATE4;
      IMED_RET ("recv server kx message", ret);

    case STATE5:
      /* receive the server certificate request - if any 
       */

      if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
	ret = _gnutls_recv_server_certificate_request (session);
      STATE = STATE5;
      IMED_RET ("recv server certificate request message", ret);

    case STATE6:
      /* receive the server hello done */
      if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
	ret =
	  _gnutls_recv_handshake (session, NULL, NULL,
				  GNUTLS_HANDSHAKE_SERVER_HELLO_DONE,
				  MANDATORY_PACKET);
      STATE = STATE6;
      IMED_RET ("recv server hello done", ret);

    case STATE71:
      if (session->security_parameters.extensions.do_send_supplemental)
	{
	  ret = _gnutls_send_supplemental (session, AGAIN (STATE71));
	  STATE = STATE71;
	  IMED_RET ("send supplemental", ret);
	}

    case STATE7:
      /* send our certificate - if any and if requested
       */
      if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
	ret = _gnutls_send_client_certificate (session, AGAIN (STATE7));
      STATE = STATE7;
      IMED_RET ("send client certificate", ret);

    case STATE8:
      if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
	ret = _gnutls_send_client_kx_message (session, AGAIN (STATE8));
      STATE = STATE8;
      IMED_RET ("send client kx", ret);

    case STATE9:
      /* send client certificate verify */
      if (session->internals.resumed == RESUME_FALSE)	/* if we are not resuming */
	ret =
	  _gnutls_send_client_certificate_verify (session, AGAIN (STATE9));
      STATE = STATE9;
      IMED_RET ("send client certificate verify", ret);

      STATE = STATE0;
    default:
      break;
    }


  return 0;
}