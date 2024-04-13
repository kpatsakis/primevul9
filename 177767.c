int gnutls_x509_ext_import_basic_constraints(const gnutls_datum_t * ext,
					  unsigned int *ca, int *pathlen)
{
	ASN1_TYPE c2 = ASN1_TYPE_EMPTY;
	char str[128]="";
	int len, result;

	if ((result = asn1_create_element
	     (_gnutls_get_pkix(), "PKIX1.BasicConstraints",
	      &c2)) != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	result = _asn1_strict_der_decode(&c2, ext->data, ext->size, NULL);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		result = _gnutls_asn2err(result);
		goto cleanup;
	}

	if (pathlen) {
		result = _gnutls_x509_read_uint(c2, "pathLenConstraint",
						(unsigned int *)
						pathlen);
		if (result == GNUTLS_E_ASN1_ELEMENT_NOT_FOUND)
			*pathlen = -1;
		else if (result != GNUTLS_E_SUCCESS) {
			gnutls_assert();
			result = _gnutls_asn2err(result);
			goto cleanup;
		}
	}

	/* the default value of cA is false.
	 */
	len = sizeof(str) - 1;
	result = asn1_read_value(c2, "cA", str, &len);
	if (result == ASN1_SUCCESS && strcmp(str, "TRUE") == 0)
		*ca = 1;
	else
		*ca = 0;

	result = 0;
 cleanup:
	asn1_delete_structure(&c2);

	return result;

}