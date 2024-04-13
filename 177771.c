int gnutls_x509_ext_import_subject_key_id(const gnutls_datum_t * ext,
				       gnutls_datum_t * id)
{
	int result, ret;
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;

	if (ext->size == 0 || ext->data == NULL) {
		gnutls_assert();
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	}

	result = asn1_create_element
	    (_gnutls_get_pkix(), "PKIX1.SubjectKeyIdentifier", &c2);
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

	ret = _gnutls_x509_read_value(c2, "", id);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = 0;
 cleanup:
	asn1_delete_structure(&c2);

	return ret;

}