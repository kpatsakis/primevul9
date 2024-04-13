_gnutls_recv_finished (gnutls_session_t session)
{
  uint8_t data[36], *vrfy;
  int data_size;
  int ret;
  int vrfysize;

  ret =
    _gnutls_recv_handshake (session, &vrfy, &vrfysize,
			    GNUTLS_HANDSHAKE_FINISHED, MANDATORY_PACKET);
  if (ret < 0)
    {
      ERR ("recv finished int", ret);
      gnutls_assert ();
      return ret;
    }


  if (gnutls_protocol_get_version (session) == GNUTLS_SSL3)
    {
      data_size = 36;
    }
  else
    {
      data_size = 12;
    }

  if (vrfysize != data_size)
    {
      gnutls_assert ();
      gnutls_free (vrfy);
      return GNUTLS_E_ERROR_IN_FINISHED_PACKET;
    }

  if (gnutls_protocol_get_version (session) == GNUTLS_SSL3)
    {
      ret =
	_gnutls_ssl3_finished (session,
			       (session->security_parameters.
				entity + 1) % 2, data);
    }
  else
    {				/* TLS 1.0 */
      ret =
	_gnutls_finished (session,
			  (session->security_parameters.entity +
			   1) % 2, data);
    }

  if (ret < 0)
    {
      gnutls_assert ();
      gnutls_free (vrfy);
      return ret;
    }

  if (memcmp (vrfy, data, data_size) != 0)
    {
      gnutls_assert ();
      ret = GNUTLS_E_ERROR_IN_FINISHED_PACKET;
    }
  gnutls_free (vrfy);

  return ret;
}