gnutls_ecc_curve_t gnutls_ecc_curve_get(gnutls_session_t session)
{
	return _gnutls_session_ecc_curve_get(session);
}