gnutls_x509_crt_get_proxy(gnutls_x509_crt_t cert,
			  unsigned int *critical,
			  int *pathlen,
			  char **policyLanguage,
			  char **policy, size_t * sizeof_policy)
{
	int result;
	gnutls_datum_t proxyCertInfo;

	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if ((result =
	     _gnutls_x509_crt_get_extension(cert, "1.3.6.1.5.5.7.1.14", 0,
					    &proxyCertInfo, critical)) < 0)
	{
		return result;
	}

	if (proxyCertInfo.size == 0 || proxyCertInfo.data == NULL) {
		gnutls_assert();
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	}

	result = gnutls_x509_ext_import_proxy(&proxyCertInfo, pathlen,
							policyLanguage,
							policy,
							sizeof_policy);
	_gnutls_free_datum(&proxyCertInfo);
	if (result < 0) {
		gnutls_assert();
		return result;
	}

	return 0;
}