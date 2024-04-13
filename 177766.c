int gnutls_x509_ext_import_private_key_usage_period(const gnutls_datum_t * ext,
						 time_t * activation,
						 time_t * expiration)
{
	int result, ret;
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;

	result = asn1_create_element
	    (_gnutls_get_pkix(), "PKIX1.PrivateKeyUsagePeriod", &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		ret = _gnutls_asn2err(result);
		goto cleanup;
	}

	result = _asn1_strict_der_decode(&c2, ext->data, ext->size, NULL);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		ret = _gnutls_asn2err(result);
		goto cleanup;
	}

	if (activation)
		*activation = _gnutls_x509_get_time(c2, "notBefore", 1);

	if (expiration)
		*expiration = _gnutls_x509_get_time(c2, "notAfter", 1);

	ret = 0;

 cleanup:
	asn1_delete_structure(&c2);

	return ret;
}