static int decode_user_notice(const void *data, size_t size,
			      gnutls_datum_t * txt)
{
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	int ret, len;
	char choice_type[64];
	char name[128];
	gnutls_datum_t td, utd;

	ret = asn1_create_element(_gnutls_get_pkix(), "PKIX1.UserNotice", &c2);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		ret = GNUTLS_E_PARSING_ERROR;
		goto cleanup;
	}

	ret = _asn1_strict_der_decode(&c2, data, size, NULL);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		ret = GNUTLS_E_PARSING_ERROR;
		goto cleanup;
	}

	len = sizeof(choice_type);
	ret = asn1_read_value(c2, "explicitText", choice_type, &len);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		ret = GNUTLS_E_PARSING_ERROR;
		goto cleanup;
	}

	if (strcmp(choice_type, "utf8String") != 0
	    && strcmp(choice_type, "ia5String") != 0
	    && strcmp(choice_type, "bmpString") != 0
	    && strcmp(choice_type, "visibleString") != 0) {
		gnutls_assert();
		ret = GNUTLS_E_PARSING_ERROR;
		goto cleanup;
	}

	snprintf(name, sizeof(name), "explicitText.%s", choice_type);

	ret = _gnutls_x509_read_value(c2, name, &td);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	if (strcmp(choice_type, "bmpString") == 0) {	/* convert to UTF-8 */
		ret = _gnutls_ucs2_to_utf8(td.data, td.size, &utd, 1);
		_gnutls_free_datum(&td);
		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}

		td.data = utd.data;
		td.size = utd.size;
	} else {
		/* _gnutls_x509_read_value allows that */
		td.data[td.size] = 0;
	}

	txt->data = (void *)td.data;
	txt->size = td.size;
	ret = 0;

 cleanup:
	asn1_delete_structure(&c2);
	return ret;

}