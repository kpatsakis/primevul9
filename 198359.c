gnutls_certificate_type_get(gnutls_session_t session)
{
	return session->security_parameters.cert_type;
}