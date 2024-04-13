gnutls_x509_crt_get_basic_constraints(gnutls_x509_crt_t cert,
				      unsigned int *critical,
				      unsigned int *ca, int *pathlen)
{
	int result;
	gnutls_datum_t basicConstraints;
	unsigned int tmp_ca;

	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if ((result =
	     _gnutls_x509_crt_get_extension(cert, "2.5.29.19", 0,
					    &basicConstraints,
					    critical)) < 0) {
		return result;
	}

	if (basicConstraints.size == 0 || basicConstraints.data == NULL) {
		gnutls_assert();
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	}

	result = gnutls_x509_ext_import_basic_constraints(&basicConstraints, &tmp_ca, pathlen);
	if (ca)
		*ca = tmp_ca;

	_gnutls_free_datum(&basicConstraints);

	if (result < 0) {
		gnutls_assert();
		return result;
	}

	return tmp_ca;
}