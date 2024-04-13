gnutls_openpgp_send_cert(gnutls_session_t session,
			 gnutls_openpgp_crt_status_t status)
{
	session->internals.pgp_fingerprint = status;
}