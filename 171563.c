gnutls_x509_crt_export(gnutls_x509_crt_t cert,
		       gnutls_x509_crt_fmt_t format, void *output_data,
		       size_t * output_data_size)
{
	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	return _gnutls_x509_export_int(cert->cert, format, PEM_X509_CERT2,
				       output_data, output_data_size);
}