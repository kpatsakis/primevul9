gnutls_x509_crt_get_authority_info_access(gnutls_x509_crt_t crt,
					  unsigned int seq,
					  int what,
					  gnutls_datum_t * data,
					  unsigned int *critical)
{
	int ret;
	gnutls_datum_t aia;
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;

	if (crt == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if ((ret =
	     _gnutls_x509_crt_get_extension(crt, GNUTLS_OID_AIA, 0, &aia,
					    critical)) < 0)
		return ret;

	if (aia.size == 0 || aia.data == NULL) {
		gnutls_assert();
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	}

	if (critical && *critical)
		return GNUTLS_E_CONSTRAINT_ERROR;

	ret = asn1_create_element(_gnutls_get_pkix(),
				  "PKIX1.AuthorityInfoAccessSyntax", &c2);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		_gnutls_free_datum(&aia);
		return _gnutls_asn2err(ret);
	}

	ret = asn1_der_decoding(&c2, aia.data, aia.size, NULL);
	/* asn1_print_structure (stdout, c2, "", ASN1_PRINT_ALL); */
	_gnutls_free_datum(&aia);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		asn1_delete_structure(&c2);
		return _gnutls_asn2err(ret);
	}

	ret = legacy_parse_aia(c2, seq, what, data);

	asn1_delete_structure(&c2);
	if (ret < 0)
		gnutls_assert();

	return ret;
}