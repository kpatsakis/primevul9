gnutls_x509_crt_get_preferred_hash_algorithm(gnutls_x509_crt_t crt,
					     gnutls_digest_algorithm_t *
					     hash, unsigned int *mand)
{
	gnutls_pk_params_st issuer_params;
	int ret;

	if (crt == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	ret = _gnutls_x509_crt_get_mpis(crt, &issuer_params);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	ret =
	    _gnutls_pk_get_hash_algorithm(gnutls_x509_crt_get_pk_algorithm
					  (crt, NULL), &issuer_params,
					  hash, mand);

	/* release allocated mpis */
	gnutls_pk_params_release(&issuer_params);

	return ret;
}