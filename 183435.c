_gnutls_send_finished (gnutls_session_t session, int again)
{
  uint8_t data[36];
  int ret;
  int data_size = 0;


  if (again == 0)
    {

      /* This is needed in order to hash all the required
       * messages.
       */
      if ((ret = _gnutls_handshake_hash_pending (session)) < 0)
	{
	  gnutls_assert ();
	  return ret;
	}

      if (gnutls_protocol_get_version (session) == GNUTLS_SSL3)
	{
	  ret =
	    _gnutls_ssl3_finished (session,
				   session->security_parameters.entity, data);
	  data_size = 36;
	}
      else
	{			/* TLS 1.0 */
	  ret =
	    _gnutls_finished (session,
			      session->security_parameters.entity, data);
	  data_size = 12;
	}

      if (ret < 0)
	{
	  gnutls_assert ();
	  return ret;
	}

    }

  ret =
    _gnutls_send_handshake (session, data, data_size,
			    GNUTLS_HANDSHAKE_FINISHED);

  return ret;
}