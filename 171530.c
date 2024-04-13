get_dn(gnutls_x509_crt_t cert, const char *whom, gnutls_x509_dn_t * dn)
{
	*dn = asn1_find_node(cert->cert, whom);
	if (!*dn)
		return GNUTLS_E_ASN1_ELEMENT_NOT_FOUND;
	return 0;
}