gnutls_x509_crt_list_import2(gnutls_x509_crt_t ** certs,
			     unsigned int *size,
			     const gnutls_datum_t * data,
			     gnutls_x509_crt_fmt_t format,
			     unsigned int flags)
{
	unsigned int init = 1024;
	int ret;

	*certs = gnutls_malloc(sizeof(gnutls_x509_crt_t) * init);
	if (*certs == NULL) {
		gnutls_assert();
		return GNUTLS_E_MEMORY_ERROR;
	}

	ret =
	    gnutls_x509_crt_list_import(*certs, &init, data, format,
					GNUTLS_X509_CRT_LIST_IMPORT_FAIL_IF_EXCEED);
	if (ret == GNUTLS_E_SHORT_MEMORY_BUFFER) {
		*certs =
		    gnutls_realloc_fast(*certs,
					sizeof(gnutls_x509_crt_t) * init);
		if (*certs == NULL) {
			gnutls_assert();
			return GNUTLS_E_MEMORY_ERROR;
		}

		ret =
		    gnutls_x509_crt_list_import(*certs, &init, data,
						format, flags);
	}

	if (ret < 0) {
		gnutls_free(*certs);
		*certs = NULL;
		return ret;
	}

	*size = init;
	return 0;
}