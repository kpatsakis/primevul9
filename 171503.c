gnutls_x509_crt_get_authority_key_id(gnutls_x509_crt_t cert, void *id,
				     size_t * id_size,
				     unsigned int *critical)
{
	int ret;
	gnutls_datum_t der, l_id;
	gnutls_x509_aki_t aki = NULL;

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

	ret = gnutls_x509_aki_get_id(aki, &l_id);

	if (ret == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
		gnutls_datum_t serial;
		ret = gnutls_x509_aki_get_cert_issuer(aki, 0, NULL, NULL, NULL, &serial);
		if (ret >= 0) {
			ret = gnutls_assert_val(GNUTLS_E_X509_UNSUPPORTED_EXTENSION);
		} else {
			ret = gnutls_assert_val(GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE);
		}
	}

	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = _gnutls_copy_data(&l_id, id, id_size);
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