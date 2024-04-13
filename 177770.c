int gnutls_x509_ext_export_key_usage(unsigned int usage, gnutls_datum_t * ext)
{
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	int result;
	uint8_t str[2];

	result = asn1_create_element(_gnutls_get_pkix(), "PKIX1.KeyUsage", &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	str[0] = usage & 0xff;
	str[1] = usage >> 8;

	/* Since KeyUsage is a BIT STRING, the input to asn1_write_value
	 * is the number of bits to be read. */
	result = asn1_write_value(c2, "", str, 9);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		asn1_delete_structure(&c2);
		return _gnutls_asn2err(result);
	}

	result = _gnutls_x509_der_encode(c2, "", ext, 0);

	asn1_delete_structure(&c2);

	if (result < 0) {
		gnutls_assert();
		return result;
	}

	return 0;
}