gnutls_x509_crt_get_policy(gnutls_x509_crt_t crt, int indx,
			   struct gnutls_x509_policy_st *policy,
			   unsigned int *critical)
{
	gnutls_datum_t tmpd = { NULL, 0 };
	int ret;
	gnutls_x509_policies_t policies = NULL;

	if (crt == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	memset(policy, 0, sizeof(*policy));

	ret = gnutls_x509_policies_init(&policies);
	if (ret < 0)
		return gnutls_assert_val(ret);

	if ((ret =
	     _gnutls_x509_crt_get_extension(crt, "2.5.29.32", 0, &tmpd,
					    critical)) < 0) {
		goto cleanup;
	}

	if (tmpd.size == 0 || tmpd.data == NULL) {
		gnutls_assert();
		ret = GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
		goto cleanup;
	}

	ret = gnutls_x509_ext_import_policies(&tmpd, policies, 0);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = gnutls_x509_policies_get(policies, indx, policy);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	_gnutls_x509_policies_erase(policies, indx);

	ret = 0;

 cleanup:
 	if (policies != NULL)
 		gnutls_x509_policies_deinit(policies);
	_gnutls_free_datum(&tmpd);

	return ret;
}