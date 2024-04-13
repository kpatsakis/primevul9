gnutls_x509_crt_get_fingerprint(gnutls_x509_crt_t cert,
				gnutls_digest_algorithm_t algo,
				void *buf, size_t * buf_size)
{
	uint8_t *cert_buf;
	int cert_buf_size;
	int result;
	gnutls_datum_t tmp;

	if (buf_size == 0 || cert == NULL) {
		return GNUTLS_E_INVALID_REQUEST;
	}

	cert_buf_size = 0;
	result = asn1_der_coding(cert->cert, "", NULL, &cert_buf_size, NULL);
	if (result != ASN1_MEM_ERROR) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	cert_buf = gnutls_malloc(cert_buf_size);
	if (cert_buf == NULL) {
		gnutls_assert();
		return GNUTLS_E_MEMORY_ERROR;
	}

	result =
	    asn1_der_coding(cert->cert, "", cert_buf, &cert_buf_size,
			    NULL);

	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		gnutls_free(cert_buf);
		return _gnutls_asn2err(result);
	}

	tmp.data = cert_buf;
	tmp.size = cert_buf_size;

	result = gnutls_fingerprint(algo, &tmp, buf, buf_size);
	gnutls_free(cert_buf);

	return result;
}