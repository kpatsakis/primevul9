gnutls_x509_crt_get_dn_oid(gnutls_x509_crt_t cert,
			   int indx, void *oid, size_t * oid_size)
{
	if (cert == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	return _gnutls_x509_get_dn_oid(cert->cert,
				       "tbsCertificate.subject.rdnSequence",
				       indx, oid, oid_size);
}