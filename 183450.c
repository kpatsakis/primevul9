_gnutls_recv_handshake_final (gnutls_session_t session, int init)
{
  int ret = 0;
  uint8_t ch;

  switch (STATE)
    {
    case STATE0:
    case STATE30:
      ret = _gnutls_recv_int (session, GNUTLS_CHANGE_CIPHER_SPEC, -1, &ch, 1);
      STATE = STATE30;
      if (ret <= 0)
	{
	  ERR ("recv ChangeCipherSpec", ret);
	  gnutls_assert ();
	  return (ret < 0) ? ret : GNUTLS_E_UNEXPECTED_PACKET_LENGTH;
	}

      /* Initialize the connection session (start encryption) - in case of server */
      if (init == TRUE)
	{
	  ret = _gnutls_connection_state_init (session);
	  if (ret < 0)
	    {
	      gnutls_assert ();
	      return ret;
	    }
	}

      ret = _gnutls_read_connection_state_init (session);
      if (ret < 0)
	{
	  gnutls_assert ();
	  return ret;
	}

    case STATE31:
      ret = _gnutls_recv_finished (session);
      STATE = STATE31;
      if (ret < 0)
	{
	  ERR ("recv finished", ret);
	  gnutls_assert ();
	  return ret;
	}
      STATE = STATE0;
    default:
      break;
    }


  return 0;
}