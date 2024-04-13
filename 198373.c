_gnutls_session_cert_type_supported(gnutls_session_t session,
				    gnutls_certificate_type_t cert_type)
{
	unsigned i;
	unsigned cert_found = 0;
	gnutls_certificate_credentials_t cred;

	if (session->security_parameters.entity == GNUTLS_SERVER) {
		cred = (gnutls_certificate_credentials_t)
		    _gnutls_get_cred(session, GNUTLS_CRD_CERTIFICATE);

		if (cred == NULL)
			return GNUTLS_E_UNSUPPORTED_CERTIFICATE_TYPE;

		if (cred->get_cert_callback == NULL && cred->get_cert_callback2 == NULL) {
			for (i = 0; i < cred->ncerts; i++) {
				if (cred->certs[i].cert_list[0].type ==
				    cert_type) {
					cert_found = 1;
					break;
				}
			}

			if (cert_found == 0)
				/* no certificate is of that type.
				 */
				return
				    GNUTLS_E_UNSUPPORTED_CERTIFICATE_TYPE;
		}
	}

	if (session->internals.priorities.cert_type.algorithms == 0
	    && cert_type == DEFAULT_CERT_TYPE)
		return 0;

	for (i = 0; i < session->internals.priorities.cert_type.algorithms;
	     i++) {
		if (session->internals.priorities.cert_type.priority[i] ==
		    cert_type) {
			return 0;	/* ok */
		}
	}

	return GNUTLS_E_UNSUPPORTED_CERTIFICATE_TYPE;
}