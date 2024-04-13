gnutls_x509_crt_get_subject_alt_othername_oid(gnutls_x509_crt_t cert,
					      unsigned int seq,
					      void *oid, size_t * oid_size)
{
	return get_alt_name(cert, "2.5.29.17", seq, oid, oid_size, NULL,
			    NULL, 1);
}