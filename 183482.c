_gnutls_set_server_random (gnutls_session_t session, uint8_t * rnd)
{
  memcpy (session->security_parameters.server_random, rnd, TLS_RANDOM_SIZE);
}