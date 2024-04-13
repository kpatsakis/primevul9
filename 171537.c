static int crt_reinit(gnutls_x509_crt_t crt)
{
	int result;

	crt->raw_dn.size = 0;
	crt->raw_issuer_dn.size = 0;

	asn1_delete_structure(&crt->cert);

	result = asn1_create_element(_gnutls_get_pkix(),
				     "PKIX1.Certificate",
				     &crt->cert);
	if (result != ASN1_SUCCESS) {
		result = _gnutls_asn2err(result);
		gnutls_assert();
		return result;
	}

	return 0;
}