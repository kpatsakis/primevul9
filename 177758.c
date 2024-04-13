int gnutls_x509_ext_export_tlsfeatures(gnutls_x509_tlsfeatures_t f,
					  gnutls_datum_t * ext)
{
	if (f == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	int ret;
	unsigned i;

	ret = asn1_create_element(_gnutls_get_pkix(), "PKIX1.TlsFeatures", &c2);
	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(ret);
	}

	for (i = 0; i < f->size; ++i) {

		ret = asn1_write_value(c2, "", "NEW", 1);
		if (ret != ASN1_SUCCESS) {
			gnutls_assert();
			ret = _gnutls_asn2err(ret);
			goto cleanup;
		}

		ret = _gnutls_x509_write_uint32(c2, "?LAST", f->feature[i]);
		if (ret != GNUTLS_E_SUCCESS) {
			gnutls_assert();
			goto cleanup;
		}
	}

	ret = _gnutls_x509_der_encode(c2, "", ext, 0);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	ret = 0;

 cleanup:
	asn1_delete_structure(&c2);
	return ret;
}