gnutls_x509_crt_verify_hash(gnutls_x509_crt_t crt, unsigned int flags,
			    const gnutls_datum_t * hash,
			    const gnutls_datum_t * signature)
{
	gnutls_pk_params_st params;
	gnutls_digest_algorithm_t algo;
	int ret;

	if (crt == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	ret = gnutls_x509_crt_get_verify_algorithm(crt, signature, &algo);
	if (ret < 0)
		return gnutls_assert_val(ret);

	/* Read the MPI parameters from the issuer's certificate.
	 */
	ret = _gnutls_x509_crt_get_mpis(crt, &params);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	ret =
	    pubkey_verify_hashed_data(gnutls_x509_crt_get_pk_algorithm
				      (crt, NULL), hash_to_entry(algo),
				      hash, signature, &params);
	if (ret < 0) {
		gnutls_assert();
	}

	/* release all allocated MPIs
	 */
	gnutls_pk_params_release(&params);

	return ret;
}