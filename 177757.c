int gnutls_x509_ext_import_authority_key_id(const gnutls_datum_t * ext,
					 gnutls_x509_aki_t aki,
					 unsigned int flags)
{
	int ret;
	unsigned i;
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	gnutls_datum_t san, othername_oid;
	unsigned type;

	ret = asn1_create_element
	    (_gnutls_get_pkix(), "PKIX1.AuthorityKeyIdentifier", &c2);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(ret);
	}

	ret = _asn1_strict_der_decode(&c2, ext->data, ext->size, NULL);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		ret = _gnutls_asn2err(ret);
		goto cleanup;
	}

	/* Read authorityCertIssuer */
	i = 0;
	do {
		san.data = NULL;
		san.size = 0;
		othername_oid.data = NULL;

		ret = _gnutls_parse_general_name2(c2, "authorityCertIssuer", i,
						  &san, &type, 0);
		if (ret < 0)
			break;

		if (type == GNUTLS_SAN_OTHERNAME) {
			ret =
			    _gnutls_parse_general_name2(c2,
							"authorityCertIssuer",
							i,
							&othername_oid,
							NULL, 1);
			if (ret < 0)
				break;
		}

		ret = subject_alt_names_set(&aki->cert_issuer.names,
					    &aki->cert_issuer.size,
					    type, &san,
					    (char *)othername_oid.data, 1);
		if (ret < 0)
			break;

		i++;
	} while (ret >= 0);

	aki->cert_issuer.size = i;
	if (ret < 0 && ret != GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE
	    && ret != GNUTLS_E_ASN1_ELEMENT_NOT_FOUND) {
		gnutls_assert();
		gnutls_free(san.data);
		gnutls_free(othername_oid.data);
		goto cleanup;
	}

	/* Read the serial number */
	ret =
	    _gnutls_x509_read_value(c2, "authorityCertSerialNumber",
				    &aki->serial);
	if (ret < 0 && ret != GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE
	    && ret != GNUTLS_E_ASN1_ELEMENT_NOT_FOUND) {
		gnutls_assert();
		goto cleanup;
	}

	/* Read the key identifier */
	ret = _gnutls_x509_read_value(c2, "keyIdentifier", &aki->id);
	if (ret < 0 && ret != GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE
	    && ret != GNUTLS_E_ASN1_ELEMENT_NOT_FOUND) {
		gnutls_assert();
		goto cleanup;
	}

	ret = 0;

 cleanup:
	asn1_delete_structure(&c2);

	return ret;
}