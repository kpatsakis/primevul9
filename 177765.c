int _gnutls_x509_decode_ext(const gnutls_datum_t *der, gnutls_x509_ext_st *out)
{
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	char str_critical[10];
	char oid[MAX_OID_SIZE];
	int result, len, ret;

	memset(out, 0, sizeof(*out));

	/* decode der */
	result = asn1_create_element(_gnutls_get_pkix(), "PKIX1.Extension", &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	result = _asn1_strict_der_decode(&c2, der->data, der->size, NULL);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		ret = _gnutls_asn2err(result);
		goto cleanup;
	}

	len = sizeof(oid)-1;
	result = asn1_read_value(c2, "extnID", oid, &len);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		ret = _gnutls_asn2err(result);
		goto cleanup;
	}

	len = sizeof(str_critical)-1;
	result = asn1_read_value(c2, "critical", str_critical, &len);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		ret = _gnutls_asn2err(result);
		goto cleanup;
	}

	if (str_critical[0] == 'T')
		out->critical = 1;
	else
		out->critical = 0;

	ret = _gnutls_x509_read_value(c2, "extnValue", &out->data);
	if (ret == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE || ret == GNUTLS_E_ASN1_ELEMENT_NOT_FOUND) {
		out->data.data = NULL;
		out->data.size = 0;
	} else if (ret < 0) {
		gnutls_assert();
		goto fail;
	}

	out->oid = gnutls_strdup(oid);
	if (out->oid == NULL) {
		ret = GNUTLS_E_MEMORY_ERROR;
		goto fail;
	}

	ret = 0;
	goto cleanup;
 fail:
	memset(out, 0, sizeof(*out));
 cleanup:
	asn1_delete_structure(&c2);
	return ret;
	
}