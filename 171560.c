gnutls_x509_crt_get_pk_algorithm(gnutls_x509_crt_t cert,
				 unsigned int *bits)
{
	int result;

	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if (bits)
		*bits = 0;

	result =
	    _gnutls_x509_get_pk_algorithm(cert->cert,
					  "tbsCertificate.subjectPublicKeyInfo",
					  bits);

	if (result < 0) {
		gnutls_assert();
		return result;
	}

	return result;

}