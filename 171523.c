gnutls_x509_crt_get_subject_alt_name2(gnutls_x509_crt_t cert,
				      unsigned int seq, void *san,
				      size_t * san_size,
				      unsigned int *san_type,
				      unsigned int *critical)
{
	return get_alt_name(cert, "2.5.29.17", seq, san, san_size,
			    san_type, critical, 0);
}