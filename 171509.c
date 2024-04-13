int gnutls_x509_crt_init(gnutls_x509_crt_t * cert)
{
	gnutls_x509_crt_t tmp;

	FAIL_IF_LIB_ERROR;

	tmp =
	    gnutls_calloc(1, sizeof(gnutls_x509_crt_int));
	int result;

	if (!tmp)
		return GNUTLS_E_MEMORY_ERROR;

	result = asn1_create_element(_gnutls_get_pkix(),
				     "PKIX1.Certificate", &tmp->cert);
	if (result != ASN1_SUCCESS) {
		gnutls_assert();
		gnutls_free(tmp);
		return _gnutls_asn2err(result);
	}

	/* If you add anything here, be sure to check if it has to be added
	   to gnutls_x509_crt_import as well. */

	*cert = tmp;

	return 0;		/* success */
}