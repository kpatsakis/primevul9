get_alt_name(gnutls_x509_crt_t cert, const char *extension_id,
	     unsigned int seq, uint8_t *alt,
	     size_t * alt_size, unsigned int *alt_type,
	     unsigned int *critical, int othername_oid)
{
	int ret;
	gnutls_datum_t dnsname = {NULL, 0};
	gnutls_datum_t ooid = {NULL, 0};
	gnutls_datum_t res;
	gnutls_subject_alt_names_t sans = NULL;
	unsigned int type;

	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if (alt == NULL)
		*alt_size = 0;

	if ((ret =
	     _gnutls_x509_crt_get_extension(cert, extension_id, 0,
					    &dnsname, critical)) < 0) {
		return ret;
	}

	if (dnsname.size == 0 || dnsname.data == NULL) {
		gnutls_assert();
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	}

	ret = gnutls_subject_alt_names_init(&sans);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = gnutls_x509_ext_import_subject_alt_names(&dnsname, sans, 0);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = gnutls_subject_alt_names_get(sans, seq, &type, &res, &ooid);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	if (othername_oid && type == GNUTLS_SAN_OTHERNAME) {
		unsigned vtype;
		gnutls_datum_t virt;
		ret = gnutls_x509_othername_to_virtual((char*)ooid.data, &res, &vtype, &virt);
		if (ret >= 0) {
			type = vtype;
			gnutls_free(res.data);
			res.data = virt.data;
			res.size = virt.size;
		}
	}

	if (alt_type)
		*alt_type = type;

	if (othername_oid) {
		ret = _gnutls_copy_string(&ooid, alt, alt_size);
	} else {
		if (is_type_printable(type)) {
			ret = _gnutls_copy_string(&res, alt, alt_size);
		} else {
			ret = _gnutls_copy_data(&res, alt, alt_size);
		}
	}

	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = type;
cleanup:
	gnutls_free(dnsname.data);
	if (sans != NULL)
		gnutls_subject_alt_names_deinit(sans);

	return ret;
}