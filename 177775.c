int gnutls_x509_ext_import_key_usage(const gnutls_datum_t * ext,
				  unsigned int *key_usage)
{
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	int len, result;
	uint8_t str[2];

	str[0] = str[1] = 0;
	*key_usage = 0;

	if ((result = asn1_create_element
	     (_gnutls_get_pkix(), "PKIX1.KeyUsage", &c2)) != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	result = _asn1_strict_der_decode(&c2, ext->data, ext->size, NULL);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		asn1_delete_structure(&c2);
		return _gnutls_asn2err(result);
	}

	len = sizeof(str);
	result = asn1_read_value(c2, "", str, &len);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		asn1_delete_structure(&c2);
		return 0;
	}

	*key_usage = str[0] | (str[1] << 8);

	asn1_delete_structure(&c2);

	return 0;
}