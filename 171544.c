gnutls_x509_crt_get_extension_by_oid2(gnutls_x509_crt_t cert,
				     const char *oid, int indx,
				     gnutls_datum_t *output,
				     unsigned int *critical)
{
	int ret;
	
	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if ((ret =
	     _gnutls_x509_crt_get_extension(cert, oid, indx, output,
					    critical)) < 0) {
		gnutls_assert();
		return ret;
	}

	if (output->size == 0 || output->data == NULL) {
		gnutls_assert();
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	}

	return 0;
}