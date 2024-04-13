gnutls_x509_crt_get_private_key_usage_period(gnutls_x509_crt_t cert,
					     time_t * activation,
					     time_t * expiration,
					     unsigned int *critical)
{
	int ret;
	gnutls_datum_t der = { NULL, 0 };

	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	ret =
	    _gnutls_x509_crt_get_extension(cert, "2.5.29.16", 0, &der,
					   critical);
	if (ret < 0)
		return gnutls_assert_val(ret);

	if (der.size == 0 || der.data == NULL)
		return
		    gnutls_assert_val
		    (GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE);

	ret = gnutls_x509_ext_import_private_key_usage_period(&der, activation, expiration);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = 0;

      cleanup:
	_gnutls_free_datum(&der);

	return ret;
}