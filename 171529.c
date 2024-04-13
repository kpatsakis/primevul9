gnutls_x509_crt_get_issuer_dn(gnutls_x509_crt_t cert, char *buf,
			      size_t * buf_size)
{
	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	return _gnutls_x509_parse_dn(cert->cert,
				     "tbsCertificate.issuer.rdnSequence",
				     buf, buf_size);
}