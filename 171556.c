gnutls_x509_crt_get_crl_dist_points(gnutls_x509_crt_t cert,
				    unsigned int seq, void *san,
				    size_t * san_size,
				    unsigned int *reason_flags,
				    unsigned int *critical)
{
	int ret;
	gnutls_datum_t dist_points = { NULL, 0 };
	unsigned type;
	gnutls_x509_crl_dist_points_t cdp = NULL;
	gnutls_datum_t t_san;

	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	ret = gnutls_x509_crl_dist_points_init(&cdp);
	if (ret < 0)
		return gnutls_assert_val(ret);

	if (reason_flags)
		*reason_flags = 0;

	ret =
	    _gnutls_x509_crt_get_extension(cert, "2.5.29.31", 0,
					   &dist_points, critical);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	if (dist_points.size == 0 || dist_points.data == NULL) {
		gnutls_assert();
		ret = GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
		goto cleanup;
	}

	ret = gnutls_x509_ext_import_crl_dist_points(&dist_points, cdp, 0);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = gnutls_x509_crl_dist_points_get(cdp, seq, &type, &t_san, reason_flags);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = _gnutls_copy_string(&t_san, san, san_size);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = type;

 cleanup:
	_gnutls_free_datum(&dist_points);
	if (cdp != NULL)
		gnutls_x509_crl_dist_points_deinit(cdp);

	return ret;
}