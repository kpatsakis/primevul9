int gnutls_x509_ext_import_subject_alt_names(const gnutls_datum_t * ext,
					  gnutls_subject_alt_names_t sans,
					  unsigned int flags)
{
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	int result, ret;
	unsigned int i;
	gnutls_datum_t san, othername_oid;
	unsigned type;

	result =
	    asn1_create_element(_gnutls_get_pkix(), "PKIX1.GeneralNames", &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	result = _asn1_strict_der_decode(&c2, ext->data, ext->size, NULL);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		ret = _gnutls_asn2err(result);
		goto cleanup;
	}

	i = 0;
	do {
		san.data = NULL;
		san.size = 0;
		othername_oid.data = NULL;

		ret = _gnutls_parse_general_name2(c2, "", i, &san, &type, 0);
		if (ret < 0)
			break;

		if (type == GNUTLS_SAN_OTHERNAME) {
			ret =
			    _gnutls_parse_general_name2(c2, "", i,
							&othername_oid,
							NULL, 1);
			if (ret < 0)
				break;

		} else if (san.size == 0 || san.data == NULL) {
			ret = gnutls_assert_val(GNUTLS_E_X509_UNKNOWN_SAN);
			break;
		}

		ret = subject_alt_names_set(&sans->names, &sans->size,
					    type, &san,
					    (char *)othername_oid.data, 1);
		if (ret < 0)
			break;

		i++;
	} while (ret >= 0);

	sans->size = i;
	if (ret < 0 && ret != GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
		gnutls_free(san.data);
		gnutls_free(othername_oid.data);
		gnutls_assert();
		goto cleanup;
	}

	ret = 0;
 cleanup:
	asn1_delete_structure(&c2);
	return ret;
}