gnutls_x509_crt_get_raw_issuer_dn(gnutls_x509_crt_t cert,
				  gnutls_datum_t * dn)
{
	if (cert->raw_issuer_dn.size > 0) {
		return _gnutls_set_datum(dn, cert->raw_issuer_dn.data,
					 cert->raw_issuer_dn.size);
	} else {
		return _gnutls_x509_get_raw_field(cert->cert, "tbsCertificate.issuer.rdnSequence", dn);
	}
}