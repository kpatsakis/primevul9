void gnutls_x509_crt_deinit(gnutls_x509_crt_t cert)
{
	if (!cert)
		return;

	if (cert->cert)
		asn1_delete_structure(&cert->cert);
	gnutls_free(cert->der.data);
	gnutls_free(cert);
}