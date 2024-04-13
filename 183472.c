_gnutls_send_handshake_final (gnutls_session_t session, int init)
{
  int ret = 0;

  /* Send the CHANGE CIPHER SPEC PACKET */

  switch (STATE)
    {
    case STATE0:
    case STATE20:
      ret = _gnutls_send_change_cipher_spec (session, AGAIN (STATE20));
      STATE = STATE20;
      if (ret < 0)
	{
	  ERR ("send ChangeCipherSpec", ret);
	  gnutls_assert ();
	  return ret;
	}

      /* Initialize the connection session (start encryption) - in case of client 
       */
      if (init == TRUE)
	{
	  ret = _gnutls_connection_state_init (session);
	  if (ret < 0)
	    {
	      gnutls_assert ();
	      return ret;
	    }
	}

      ret = _gnutls_write_connection_state_init (session);
      if (ret < 0)
	{
	  gnutls_assert ();
	  return ret;
	}

    case STATE21:
      /* send the finished message */
      ret = _gnutls_send_finished (session, AGAIN (STATE21));
      STATE = STATE21;
      if (ret < 0)
	{
	  ERR ("send Finished", ret);
	  gnutls_assert ();
	  return ret;
	}

      STATE = STATE0;
    default:
      break;
    }

  return 0;
}