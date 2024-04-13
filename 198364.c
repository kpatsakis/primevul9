int _gnutls_session_is_psk(gnutls_session_t session)
{
	gnutls_kx_algorithm_t kx;

	kx = _gnutls_cipher_suite_get_kx_algo(session->security_parameters.
					      cipher_suite);
	if (kx == GNUTLS_KX_PSK || kx == GNUTLS_KX_DHE_PSK
	    || kx == GNUTLS_KX_RSA_PSK)
		return 1;

	return 0;
}