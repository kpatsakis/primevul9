gnutls_x509_crt_get_pk_dsa_raw(gnutls_x509_crt_t crt,
			       gnutls_datum_t * p, gnutls_datum_t * q,
			       gnutls_datum_t * g, gnutls_datum_t * y)
{
	int ret;
	gnutls_pk_params_st params;

	if (crt == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	ret = gnutls_x509_crt_get_pk_algorithm(crt, NULL);
	if (ret != GNUTLS_PK_DSA) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	ret = _gnutls_x509_crt_get_mpis(crt, &params);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}


	/* P */
	ret = _gnutls_mpi_dprint_lz(params.params[0], p);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	/* Q */
	ret = _gnutls_mpi_dprint_lz(params.params[1], q);
	if (ret < 0) {
		gnutls_assert();
		_gnutls_free_datum(p);
		goto cleanup;
	}


	/* G */
	ret = _gnutls_mpi_dprint_lz(params.params[2], g);
	if (ret < 0) {
		gnutls_assert();
		_gnutls_free_datum(p);
		_gnutls_free_datum(q);
		goto cleanup;
	}


	/* Y */
	ret = _gnutls_mpi_dprint_lz(params.params[3], y);
	if (ret < 0) {
		gnutls_assert();
		_gnutls_free_datum(p);
		_gnutls_free_datum(g);
		_gnutls_free_datum(q);
		goto cleanup;
	}

	ret = 0;

      cleanup:
	gnutls_pk_params_release(&params);
	return ret;

}