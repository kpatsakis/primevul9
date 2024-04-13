int gnutls_x509_crt_get_dn2(gnutls_x509_crt_t cert, gnutls_datum_t * dn)
{
	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	return _gnutls_x509_get_dn(cert->cert,
				   "tbsCertificate.subject.rdnSequence",
				   dn);
}