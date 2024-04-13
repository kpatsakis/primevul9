crl_issuer_matches(gnutls_x509_crl_t crl, gnutls_x509_crt_t cert)
{
	if (_gnutls_x509_compare_raw_dn
	    (&crl->raw_issuer_dn, &cert->raw_issuer_dn) != 0)
		return 1;
	else
		return 0;
}