static int parse_tlsfeatures(ASN1_TYPE c2, gnutls_x509_tlsfeatures_t f, unsigned flags)
{
	char nptr[ASN1_MAX_NAME_SIZE];
	int result;
	unsigned i, indx, j;
	unsigned int feature;

	if (!(flags & GNUTLS_EXT_FLAG_APPEND))
		f->size = 0;

	for (i = 1;; i++) {
		unsigned skip = 0;
		snprintf(nptr, sizeof(nptr), "?%u", i);

		result = _gnutls_x509_read_uint(c2, nptr, &feature);

		if (result == GNUTLS_E_ASN1_ELEMENT_NOT_FOUND || result == GNUTLS_E_ASN1_VALUE_NOT_FOUND) {
			break;
		}
		else if (result != GNUTLS_E_SUCCESS) {
			gnutls_assert();
			return _gnutls_asn2err(result);
		}

		if (feature > UINT16_MAX) {
			gnutls_assert();
			return GNUTLS_E_CERTIFICATE_ERROR;
		}

		/* skip duplicates */
		for (j=0;j<f->size;j++) {
			if (f->feature[j] == feature) {
				skip = 1;
				break;
			}
		}

		if (!skip) {
			if (f->size >= sizeof(f->feature)/sizeof(f->feature[0])) {
				gnutls_assert();
				return GNUTLS_E_INTERNAL_ERROR;
			}

			indx = f->size;
			f->feature[indx] = feature;
			f->size++;
		}
	}

	return 0;
}