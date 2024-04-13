gnutls_x509_crt_get_extension_data(gnutls_x509_crt_t cert, int indx,
				   void *data, size_t * sizeof_data)
{
	int result, len;
	char name[ASN1_MAX_NAME_SIZE];

	if (!cert) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	snprintf(name, sizeof(name),
		 "tbsCertificate.extensions.?%u.extnValue", indx + 1);

	len = *sizeof_data;
	result = asn1_read_value(cert->cert, name, data, &len);
	*sizeof_data = len;

	if (result == ASN1_ELEMENT_NOT_FOUND) {
		return GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE;
	} else if (result == ASN1_MEM_ERROR && data == NULL) {
		/* normally we should return GNUTLS_E_SHORT_MEMORY_BUFFER,
		 * but we haven't done that for long time, so use
		 * backwards compatible behavior */
		return 0;
	} else if (result != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	return 0;
}