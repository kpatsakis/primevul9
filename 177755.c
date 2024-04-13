static int parse_aia(ASN1_TYPE c2, gnutls_x509_aia_t aia)
{
	int len;
	char nptr[ASN1_MAX_NAME_SIZE];
	int ret, result;
	char tmpoid[MAX_OID_SIZE];
	void * tmp;
	unsigned i, indx;

	for (i = 1;; i++) {
		snprintf(nptr, sizeof(nptr), "?%u.accessMethod", i);

		len = sizeof(tmpoid);
		result = asn1_read_value(c2, nptr, tmpoid, &len);
		if (result == ASN1_VALUE_NOT_FOUND
		    || result == ASN1_ELEMENT_NOT_FOUND) {
			ret = GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
			break;
		}

		if (result != ASN1_SUCCESS) {
			gnutls_assert();
			return _gnutls_asn2err(result);
		}

		indx = aia->size;
		tmp = gnutls_realloc(aia->aia, (aia->size + 1) * sizeof(aia->aia[0]));
		if (tmp == NULL) {
			return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);
		}
		aia->aia = tmp;

		aia->aia[indx].oid.data = (void*)gnutls_strdup(tmpoid);
		aia->aia[indx].oid.size = strlen(tmpoid);

		snprintf(nptr, sizeof(nptr), "?%u.accessLocation", i);

		ret = _gnutls_parse_general_name2(c2, nptr, -1, &aia->aia[indx].san, 
			&aia->aia[indx].san_type, 0);
		if (ret < 0)
			break;

		aia->size++;

	}
	
	if (ret < 0 && ret != GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
		return ret;
	}

	return 0;
}