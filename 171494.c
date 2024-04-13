gnutls_x509_crt_get_issuer_alt_name(gnutls_x509_crt_t cert,
				    unsigned int seq, void *ian,
				    size_t * ian_size,
				    unsigned int *critical)
{
	return get_alt_name(cert, "2.5.29.18", seq, ian, ian_size, NULL,
			    critical, 0);
}