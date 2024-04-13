gnutls_x509_crt_get_key_purpose_oid(gnutls_x509_crt_t cert,
				    int indx, void *oid, size_t * oid_size,
				    unsigned int *critical)
{
	int ret;
	gnutls_datum_t ext;
	gnutls_x509_key_purposes_t p = NULL;
	gnutls_datum_t out;

	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if (oid)
		memset(oid, 0, *oid_size);
	else
		*oid_size = 0;

	if ((ret =
	     _gnutls_x509_crt_get_extension(cert, "2.5.29.37", 0, &ext,
					    critical)) < 0) {
		return ret;
	}

	if (ext.size == 0 || ext.data == NULL) {
		gnutls_assert();
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	}

	ret = gnutls_x509_key_purpose_init(&p);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = gnutls_x509_ext_import_key_purposes(&ext, p, 0);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = gnutls_x509_key_purpose_get(p, indx, &out);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = _gnutls_copy_data(&out, oid, oid_size);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = 0;

 cleanup:
 	gnutls_free(ext.data);
 	if (p!=NULL)
 		gnutls_x509_key_purpose_deinit(p);
	return ret;
}