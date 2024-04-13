gnutls_x509_crt_get_signature(gnutls_x509_crt_t cert,
			      char *sig, size_t * sig_size)
{
	gnutls_datum_t dsig = {NULL, 0};
	int ret;

	if (cert == NULL)
		return gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);

	ret = _gnutls_x509_get_signature(cert->cert, "signature", &dsig);
	if (ret < 0)
		return gnutls_assert_val(ret);

	ret = _gnutls_copy_data(&dsig, (uint8_t*)sig, sig_size);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = 0;
 cleanup:
 	gnutls_free(dsig.data);
 	return ret;
}