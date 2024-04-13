_gnutls_get_key_id(gnutls_pk_algorithm_t pk, gnutls_pk_params_st * params,
		   unsigned char *output_data, size_t * output_data_size)
{
	int ret = 0;
	gnutls_datum_t der = { NULL, 0 };
	const gnutls_digest_algorithm_t hash = GNUTLS_DIG_SHA1;
	unsigned int digest_len =
	    _gnutls_hash_get_algo_len(hash_to_entry(hash));

	if (output_data == NULL || *output_data_size < digest_len) {
		gnutls_assert();
		*output_data_size = digest_len;
		return GNUTLS_E_SHORT_MEMORY_BUFFER;
	}

	ret = _gnutls_x509_encode_PKI_params(&der, pk, params);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ret = _gnutls_hash_fast(hash, der.data, der.size, output_data);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}
	*output_data_size = digest_len;

	ret = 0;

      cleanup:

	_gnutls_free_datum(&der);
	return ret;
}