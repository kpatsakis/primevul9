gnutls_x509_crt_get_issuer(gnutls_x509_crt_t cert, gnutls_x509_dn_t * dn)
{
	return get_dn(cert, "tbsCertificate.issuer.rdnSequence", dn);
}