gnutls_x509_crt_get_pk_rsa_raw(gnutls_x509_crt_t crt,
			       gnutls_datum_t * m, gnutls_datum_t * e)
{
	int ret;
	gnutls_pk_params_st params;

	if (crt == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	ret = gnutls_x509_crt_get_pk_algorithm(crt, NULL);
	if (ret != GNUTLS_PK_RSA) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	ret = _gnutls_x509_crt_get_mpis(crt, &params);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	ret = _gnutls_mpi_dprint_lz(params.params[0], m);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = _gnutls_mpi_dprint_lz(params.params[1], e);
	if (ret < 0) {
		gnutls_assert();
		_gnutls_free_datum(m);
		goto cleanup;
	}

	ret = 0;

      cleanup:
	gnutls_pk_params_release(&params);
	return ret;
}