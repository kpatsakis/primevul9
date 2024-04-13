gnutls_x509_crt_get_subject(gnutls_x509_crt_t cert, gnutls_x509_dn_t * dn)
{
	return get_dn(cert, "tbsCertificate.subject.rdnSequence", dn);
}