gnutls_x509_crt_get_extension_by_oid(gnutls_x509_crt_t cert,
				     const char *oid, int indx,
				     void *buf, size_t * buf_size,
				     unsigned int *critical)
{
	int result;
	gnutls_datum_t output;

	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if ((result =
	     _gnutls_x509_crt_get_extension(cert, oid, indx, &output,
					    critical)) < 0) {
		gnutls_assert();
		return result;
	}

	if (output.size == 0 || output.data == NULL) {
		gnutls_assert();
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	}

	if (output.size > (unsigned int) *buf_size) {
		*buf_size = output.size;
		_gnutls_free_datum(&output);
		return GNUTLS_E_SHORT_MEMORY_BUFFER;
	}

	*buf_size = output.size;

	if (buf)
		memcpy(buf, output.data, output.size);

	_gnutls_free_datum(&output);

	return 0;
}