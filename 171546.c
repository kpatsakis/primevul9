gnutls_x509_crt_get_key_id(gnutls_x509_crt_t crt, unsigned int flags,
			   unsigned char *output_data,
			   size_t * output_data_size)
{
	int pk, ret = 0;
	gnutls_pk_params_st params;

	if (crt == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	pk = gnutls_x509_crt_get_pk_algorithm(crt, NULL);
	if (pk < 0) {
		gnutls_assert();
		return pk;
	}

	ret = _gnutls_x509_crt_get_mpis(crt, &params);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	ret =
	    _gnutls_get_key_id(pk, &params, output_data, output_data_size);

	gnutls_pk_params_release(&params);

	return ret;
}