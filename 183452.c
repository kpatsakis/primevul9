_gnutls_send_hello (gnutls_session_t session, int again)
{
  int ret;

  if (session->security_parameters.entity == GNUTLS_CLIENT)
    {
      ret = _gnutls_send_client_hello (session, again);

    }
  else
    {				/* SERVER */
      ret = _gnutls_send_server_hello (session, again);
    }

  return ret;
}