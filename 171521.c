int gnutls_x509_crt_get_raw_dn(gnutls_x509_crt_t cert, gnutls_datum_t * dn)
{
	if (cert->raw_dn.size > 0) {
		return _gnutls_set_datum(dn, cert->raw_dn.data, cert->raw_dn.size);
	} else {
		return _gnutls_x509_get_raw_field(cert->cert, "tbsCertificate.subject.rdnSequence", dn);
	}
}