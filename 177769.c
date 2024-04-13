int gnutls_x509_ext_export_subject_alt_names(gnutls_subject_alt_names_t sans,
					  gnutls_datum_t * ext)
{
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	int result, ret;
	unsigned i;

	result =
	    asn1_create_element(_gnutls_get_pkix(), "PKIX1.GeneralNames", &c2);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	for (i = 0; i < sans->size; i++) {
		if (sans->names[i].type == GNUTLS_SAN_OTHERNAME) {
			ret = _gnutls_write_new_othername(c2, "", (char*)sans->names[i].othername_oid.data,
					      sans->names[i].san.data, sans->names[i].san.size);
		} else {
			ret =
			    _gnutls_write_new_general_name(c2, "", sans->names[i].type,
							   sans->names[i].san.data,
							   sans->names[i].san.size);
		}

		if (ret < 0) {
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