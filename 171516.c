gnutls_x509_crt_get_issuer_unique_id(gnutls_x509_crt_t crt, char *buf,
				     size_t * buf_size)
{
	int result;
	gnutls_datum_t datum = { NULL, 0 };

	result =
	    _gnutls_x509_read_value(crt->cert,
				    "tbsCertificate.issuerUniqueID",
				    &datum);

	if (datum.size > *buf_size) {	/* then we're not going to fit */
		*buf_size = datum.size;
		buf[0] = '\0';
		result = GNUTLS_E_SHORT_MEMORY_BUFFER;
	} else {
		*buf_size = datum.size;
		memcpy(buf, datum.data, datum.size);
	}

	_gnutls_free_datum(&datum);

	return result;
}