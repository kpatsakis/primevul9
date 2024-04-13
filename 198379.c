_gnutls_session_ecc_curve_set(gnutls_session_t session,
			      gnutls_ecc_curve_t c)
{
	_gnutls_handshake_log("HSK[%p]: Selected ECC curve %s (%d)\n",
			      session, gnutls_ecc_curve_get_name(c), c);
	session->security_parameters.ecc_curve = c;
}