int gnutls_x509_crt_get_signature_algorithm(gnutls_x509_crt_t cert)
{
	return _gnutls_x509_get_signature_algorithm(cert->cert,
						    "signatureAlgorithm.algorithm");
}