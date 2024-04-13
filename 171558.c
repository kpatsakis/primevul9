gnutls_x509_crt_get_extension_info(gnutls_x509_crt_t cert, int indx,
				   void *oid, size_t * oid_size,
				   unsigned int *critical)
{
	int result;
	char str_critical[10];
	char name[ASN1_MAX_NAME_SIZE];
	int len;

	if (!cert) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	snprintf(name, sizeof(name),
		 "tbsCertificate.extensions.?%u.extnID", indx + 1);

	len = *oid_size;
	result = asn1_read_value(cert->cert, name, oid, &len);
	*oid_size = len;

	if (result == ASN1_ELEMENT_NOT_FOUND)
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	else if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	snprintf(name, sizeof(name),
		 "tbsCertificate.extensions.?%u.critical", indx + 1);
	len = sizeof(str_critical);
	result = asn1_read_value(cert->cert, name, str_critical, &len);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	if (critical) {
		if (str_critical[0] == 'T')
			*critical = 1;
		else
			*critical = 0;
	}

	return 0;

}