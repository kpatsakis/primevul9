_gnutls_parse_general_name(ASN1_TYPE src, const char *src_name,
			   int seq, void *name, size_t * name_size,
			   unsigned int *ret_type, int othername_oid)
{
	int ret;
	gnutls_datum_t res = {NULL,0};
	unsigned type;

	ret = _gnutls_parse_general_name2(src, src_name, seq, &res, ret_type, othername_oid);
	if (ret < 0)
		return gnutls_assert_val(ret);

	type = ret;

	if (is_type_printable(type)) {
		ret = _gnutls_copy_string(&res, name, name_size);
	} else {
		ret = _gnutls_copy_data(&res, name, name_size);
	}

	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = type;
cleanup:
	gnutls_free(res.data);
	return ret;
}