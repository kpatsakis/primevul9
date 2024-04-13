gnutls_x509_crt_get_key_usage(gnutls_x509_crt_t cert,
			      unsigned int *key_usage,
			      unsigned int *critical)
{
	int result;
	gnutls_datum_t keyUsage;

	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if ((result =
	     _gnutls_x509_crt_get_extension(cert, "2.5.29.15", 0,
					    &keyUsage, critical)) < 0) {
		return result;
	}

	if (keyUsage.size == 0 || keyUsage.data == NULL) {
		gnutls_assert();
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	}

	result = gnutls_x509_ext_import_key_usage(&keyUsage, key_usage);
	_gnutls_free_datum(&keyUsage);

	if (result < 0) {
		gnutls_assert();
		return result;
	}

	return 0;
}