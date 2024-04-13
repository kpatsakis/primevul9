_gnutls_parse_general_name2(ASN1_TYPE src, const char *src_name,
			   int seq, gnutls_datum_t *dname, 
			   unsigned int *ret_type, int othername_oid)
{
	int len, ret;
	char nptr[ASN1_MAX_NAME_SIZE];
	int result;
	gnutls_datum_t tmp = {NULL, 0};
	char choice_type[128];
	gnutls_x509_subject_alt_name_t type;

	if (seq != -1) {
		seq++;	/* 0->1, 1->2 etc */

		if (src_name[0] != 0)
			snprintf(nptr, sizeof(nptr), "%s.?%u", src_name, seq);
		else
			snprintf(nptr, sizeof(nptr), "?%u", seq);
	} else {
		snprintf(nptr, sizeof(nptr), "%s", src_name);
	}

	len = sizeof(choice_type);
	result = asn1_read_value(src, nptr, choice_type, &len);

	if (result == ASN1_VALUE_NOT_FOUND
	    || result == ASN1_ELEMENT_NOT_FOUND) {
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	}

	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	type = _gnutls_x509_san_find_type(choice_type);
	if (type == (gnutls_x509_subject_alt_name_t) - 1) {
		gnutls_assert();
		return GNUTLS_E_X509_UNKNOWN_SAN;
	}

	if (ret_type)
		*ret_type = type;

	if (type == GNUTLS_SAN_OTHERNAME) {
		if (othername_oid)
			_gnutls_str_cat(nptr, sizeof(nptr),
					".otherName.type-id");
		else
			_gnutls_str_cat(nptr, sizeof(nptr),
					".otherName.value");

		ret = _gnutls_x509_read_value(src, nptr, &tmp);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}

		if (othername_oid) {
			dname->size = tmp.size;
			dname->data = tmp.data;
		} else {
			char oid[MAX_OID_SIZE];

			if (src_name[0] != 0)
				snprintf(nptr, sizeof(nptr),
					 "%s.?%u.otherName.type-id",
					 src_name, seq);
			else
				snprintf(nptr, sizeof(nptr),
					 "?%u.otherName.type-id", seq);

			len = sizeof(oid);

			result = asn1_read_value(src, nptr, oid, &len);
			if (result != ASN1_SUCCESS) {
				gnutls_assert();
				ret = _gnutls_asn2err(result);
				goto cleanup;
			}
			if (len > 0) len--;

			dname->size = tmp.size;
			dname->data = tmp.data;
		}
	} else if (type == GNUTLS_SAN_DN) {
		_gnutls_str_cat(nptr, sizeof(nptr), ".directoryName");
		ret = _gnutls_x509_get_dn(src, nptr, dname);
		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}
	} else if (othername_oid) {
		gnutls_assert();
		ret = GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
		goto cleanup;
	} else {
		_gnutls_str_cat(nptr, sizeof(nptr), ".");
		_gnutls_str_cat(nptr, sizeof(nptr), choice_type);

		ret = _gnutls_x509_read_value(src, nptr, &tmp);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}

		/* _gnutls_x509_read_value() null terminates */
		dname->size = tmp.size;
		dname->data = tmp.data;
	}

	return type;

 cleanup:
	gnutls_free(tmp.data);
	return ret;
}