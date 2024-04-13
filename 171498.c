gnutls_x509_crt_get_subject_key_id(gnutls_x509_crt_t cert, void *ret,
				   size_t * ret_size,
				   unsigned int *critical)
{
	int result;
	gnutls_datum_t id = {NULL,0};
	gnutls_datum_t der = {NULL, 0};

	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if (ret == NULL)
		*ret_size = 0;

	if ((result =
	     _gnutls_x509_crt_get_extension(cert, "2.5.29.14", 0, &der,
					    critical)) < 0) {
		return result;
	}

	result = gnutls_x509_ext_import_subject_key_id(&der, &id);
	if (result < 0) {
		gnutls_assert();
		goto cleanup;
	}

	result = _gnutls_copy_data(&id, ret, ret_size);
	if (result < 0) {
		gnutls_assert();
		goto cleanup;
	}

	result = 0;

 cleanup:
	gnutls_free(der.data);
	gnutls_free(id.data);
	return result;
}