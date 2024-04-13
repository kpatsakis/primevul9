_gnutls_set_client_random (gnutls_session_t session, uint8_t * rnd)
{
  memcpy (session->security_parameters.client_random, rnd, TLS_RANDOM_SIZE);
}