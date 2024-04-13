int gnutls_x509_ext_import_key_purposes(const gnutls_datum_t * ext,
				     gnutls_x509_key_purposes_t p,
				     unsigned int flags)
{
	char tmpstr[ASN1_MAX_NAME_SIZE];
	int result, ret;
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	gnutls_datum_t oid = {NULL, 0};
	unsigned i;

	result = asn1_create_element
	    (_gnutls_get_pkix(), "PKIX1.ExtKeyUsageSyntax", &c2);
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

	key_purposes_deinit(p);
	i = 0;
	p->size = 0;

	for (;i<MAX_ENTRIES;i++) {
		/* create a string like "?1"
		 */
		snprintf(tmpstr, sizeof(tmpstr), "?%u", i+1);

		ret = _gnutls_x509_read_value(c2, tmpstr, &oid);
		if (ret == GNUTLS_E_ASN1_ELEMENT_NOT_FOUND) {
			ret = 0;
			break;
		}

		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}

		p->oid[i].data = oid.data;
		p->oid[i].size = oid.size;

		oid.data = NULL;
		oid.size = 0;
		p->size++;
	}

	ret = 0;
 cleanup:
	gnutls_free(oid.data);
	asn1_delete_structure(&c2);

	return ret;

}