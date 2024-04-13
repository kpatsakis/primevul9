_gnutls_abort_handshake (gnutls_session_t session, int ret)
{
  if (((ret == GNUTLS_E_WARNING_ALERT_RECEIVED) &&
       (gnutls_alert_get (session) == GNUTLS_A_NO_RENEGOTIATION))
      || ret == GNUTLS_E_GOT_APPLICATION_DATA)
    return 0;

  /* this doesn't matter */
  return GNUTLS_E_INTERNAL_ERROR;
}