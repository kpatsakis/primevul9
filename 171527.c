gnutls_x509_crt_export2(gnutls_x509_crt_t cert,
			gnutls_x509_crt_fmt_t format, gnutls_datum_t * out)
{
	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	return _gnutls_x509_export_int2(cert->cert, format, PEM_X509_CERT2,
					out);
}