gnutls_x509_crt_get_ca_status(gnutls_x509_crt_t cert,
			      unsigned int *critical)
{
	int pathlen;
	unsigned int ca;
	return gnutls_x509_crt_get_basic_constraints(cert, critical, &ca,
						     &pathlen);
}