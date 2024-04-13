gnutls_x509_crt_get_authority_key_gn_serial(gnutls_x509_crt_t cert,
					    unsigned int seq, void *alt,
					    size_t * alt_size,
					    unsigned int *alt_type,
					    void *serial,
					    size_t * serial_size,
					    unsigned int *critical)
{
	int ret;
	gnutls_datum_t der, san, iserial;
	gnutls_x509_aki_t aki = NULL;
	unsigned san_type;

	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if ((ret =
	     _gnutls_x509_crt_get_extension(cert, "2.5.29.35", 0, &der,
					    critical)) < 0) {
		return gnutls_assert_val(ret);
	}

	if (der.size == 0 || der.data == NULL) {
		gnutls_assert();
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	}

	ret = gnutls_x509_aki_init(&aki);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = gnutls_x509_ext_import_authority_key_id(&der, aki, 0);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = gnutls_x509_aki_get_cert_issuer(aki, seq, &san_type, &san, NULL, &iserial);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	if (is_type_printable(san_type))
		ret = _gnutls_copy_string(&san, alt, alt_size);
	else
		ret = _gnutls_copy_data(&san, alt, alt_size);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	if (alt_type)
		*alt_type = san_type;

	ret = _gnutls_copy_data(&iserial, serial, serial_size);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = 0;
 cleanup:
 	if (aki != NULL)
 		gnutls_x509_aki_deinit(aki);
 	gnutls_free(der.data);
 	return ret;
}