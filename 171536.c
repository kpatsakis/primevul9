int gnutls_x509_crt_get_version(gnutls_x509_crt_t cert)
{
	uint8_t version[8];
	int len, result;

	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	len = sizeof(version);
	if ((result =
	     asn1_read_value(cert->cert, "tbsCertificate.version", version,
			     &len)) != ASN1_SUCCESS) {

		if (result == ASN1_ELEMENT_NOT_FOUND)
			return 1;	/* the DEFAULT version */
		gnutls_assert();
		return _gnutls_asn2err(result);
	}

	return (int) version[0] + 1;
}