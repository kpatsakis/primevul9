gnutls_x509_crt_verify_data(gnutls_x509_crt_t crt, unsigned int flags,
			    const gnutls_datum_t * data,
			    const gnutls_datum_t * signature)
{
	int result;

	if (crt == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	result =
	    _gnutls_x509_verify_data(GNUTLS_DIG_UNKNOWN, data, signature,
				     crt);
	if (result < 0) {
		gnutls_assert();
		return result;
	}

	return result;
}