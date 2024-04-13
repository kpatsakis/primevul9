int gnutls_x509_ext_import_tlsfeatures(const gnutls_datum_t * ext,
				       gnutls_x509_tlsfeatures_t f,
				       unsigned int flags)
{
	int ret;
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;

	if (ext->size == 0 || ext->data == NULL) {
		gnutls_assert();
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	}

	ret = asn1_create_element(_gnutls_get_pkix(),
				  "PKIX1.TlsFeatures", &c2);
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

	ret = parse_tlsfeatures(c2, f, flags);
	if (ret < 0) {
		gnutls_assert();
	}

 cleanup:
	asn1_delete_structure(&c2);

	return ret;
}