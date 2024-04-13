gnutls_x509_crt_get_issuer_alt_name2(gnutls_x509_crt_t cert,
				     unsigned int seq, void *ian,
				     size_t * ian_size,
				     unsigned int *ian_type,
				     unsigned int *critical)
{
	return get_alt_name(cert, "2.5.29.18", seq, ian, ian_size,
			    ian_type, critical, 0);
}