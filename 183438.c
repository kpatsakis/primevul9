_gnutls_recv_hello (gnutls_session_t session, opaque * data, int datalen)
{
  int ret;

  if (session->security_parameters.entity == GNUTLS_CLIENT)
    {
      ret = _gnutls_read_server_hello (session, data, datalen);
      if (ret < 0)
	{
	  gnutls_assert ();
	  return ret;
	}
    }
  else
    {				/* Server side reading a client hello */

      ret = _gnutls_read_client_hello (session, data, datalen);
      if (ret < 0)
	{
	  gnutls_assert ();
	  return ret;
	}
    }

  return ret;
}