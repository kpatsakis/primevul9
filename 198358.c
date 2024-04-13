gnutls_kx_algorithm_t gnutls_kx_get(gnutls_session_t session)
{
	return session->security_parameters.kx_algorithm;
}