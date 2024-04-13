_gnutls_session_cert_type_set(gnutls_session_t session,
			      gnutls_certificate_type_t ct)
{
	_gnutls_handshake_log
	    ("HSK[%p]: Selected certificate type %s (%d)\n", session,
	     gnutls_certificate_type_get_name(ct), ct);
	session->security_parameters.cert_type = ct;
}