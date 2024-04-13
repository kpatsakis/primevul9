int _gnutls_x509_crt_cpy(gnutls_x509_crt_t dest, gnutls_x509_crt_t src)
{
	int ret;
	size_t der_size = 0;
	uint8_t *der;
	gnutls_datum_t tmp;

	ret =
	    gnutls_x509_crt_export(src, GNUTLS_X509_FMT_DER, NULL,
				   &der_size);
	if (ret != GNUTLS_E_SHORT_MEMORY_BUFFER) {
		gnutls_assert();
		return ret;
	}

	der = gnutls_malloc(der_size);
	if (der == NULL) {
		gnutls_assert();
		return GNUTLS_E_MEMORY_ERROR;
	}

	ret =
	    gnutls_x509_crt_export(src, GNUTLS_X509_FMT_DER, der,
				   &der_size);
	if (ret < 0) {
		gnutls_assert();
		gnutls_free(der);
		return ret;
	}

	tmp.data = der;
	tmp.size = der_size;
	ret = gnutls_x509_crt_import(dest, &tmp, GNUTLS_X509_FMT_DER);

	gnutls_free(der);

	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	return 0;
}