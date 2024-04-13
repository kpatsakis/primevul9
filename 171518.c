gnutls_x509_crt_check_revocation(gnutls_x509_crt_t cert,
				 const gnutls_x509_crl_t * crl_list,
				 int crl_list_length)
{
	return _gnutls_x509_crt_check_revocation(cert, crl_list,
						 crl_list_length, NULL);
}