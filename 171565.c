gnutls_x509_crt_get_serial(gnutls_x509_crt_t cert, void *result,
			   size_t * result_size)
{
	int ret, len;

	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	len = *result_size;
	ret =
	    asn1_read_value(cert->cert, "tbsCertificate.serialNumber",
			    result, &len);
	*result_size = len;

	if (ret != ASN1_SUCCESS) {
		gnutls_assert();
		return _gnutls_asn2err(ret);
	}

	return 0;
}