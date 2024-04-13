legacy_parse_aia(ASN1_TYPE src,
		  unsigned int seq, int what, gnutls_datum_t * data)
{
	int len;
	char nptr[ASN1_MAX_NAME_SIZE];
	int result;
	gnutls_datum_t d;
	const char *oid = NULL;

	seq++;			/* 0->1, 1->2 etc */
	switch (what) {
	case GNUTLS_IA_ACCESSMETHOD_OID:
		snprintf(nptr, sizeof(nptr), "?%u.accessMethod", seq);
		break;

	case GNUTLS_IA_ACCESSLOCATION_GENERALNAME_TYPE:
		snprintf(nptr, sizeof(nptr), "?%u.accessLocation", seq);
		break;

	case GNUTLS_IA_CAISSUERS_URI:
		oid = GNUTLS_OID_AD_CAISSUERS;
		/* fall through */

	case GNUTLS_IA_OCSP_URI:
		if (oid == NULL)
			oid = GNUTLS_OID_AD_OCSP;
		{
			char tmpoid[MAX_OID_SIZE];
			snprintf(nptr, sizeof(nptr), "?%u.accessMethod",
				 seq);
			len = sizeof(tmpoid);
			result = asn1_read_value(src, nptr, tmpoid, &len);

			if (result == ASN1_VALUE_NOT_FOUND
			    || result == ASN1_ELEMENT_NOT_FOUND)
				return
				    gnutls_assert_val
				    (GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE);

			if (result != ASN1_SUCCESS) {
				gnutls_assert();
				return _gnutls_asn2err(result);
			}
			if ((unsigned) len != strlen(oid) + 1
			    || memcmp(tmpoid, oid, len) != 0)
				return
				    gnutls_assert_val
				    (GNUTLS_E_UNKNOWN_ALGORITHM);
		}
		/* fall through */

	case GNUTLS_IA_URI:
		snprintf(nptr, sizeof(nptr),
			 "?%u.accessLocation.uniformResourceIdentifier",
			 seq);
		break;

	default:
		return gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);
	}

	len = 0;
	result = asn1_read_value(src, nptr, NULL, &len);
	if (result == ASN1_VALUE_NOT_FOUND
	    || result == ASN1_ELEMENT_NOT_FOUND)
		return
		    gnutls_assert_val
		    (GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE);

	if (result != ASN1_MEM_ERROR) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	d.size = len;

	d.data = gnutls_malloc(d.size);
	if (d.data == NULL)
		return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);

	result = asn1_read_value(src, nptr, d.data, &len);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		gnutls_free(d.data);
		return _gnutls_asn2err(result);
	}

	if (data) {
		data->data = d.data;
		data->size = d.size;
	} else
		gnutls_free(d.data);

	return 0;
}