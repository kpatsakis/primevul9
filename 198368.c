int _gnutls_openpgp_send_fingerprint(gnutls_session_t session)
{
	return session->internals.pgp_fingerprint;
}