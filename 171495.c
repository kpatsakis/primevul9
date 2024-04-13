time_t gnutls_x509_crt_get_activation_time(gnutls_x509_crt_t cert)
{
	if (cert == NULL) {
		gnutls_assert();
		return (time_t) - 1;
	}

	return _gnutls_x509_get_time(cert->cert,
				     "tbsCertificate.validity.notBefore",
				     0);
}