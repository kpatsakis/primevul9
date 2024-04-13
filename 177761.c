int gnutls_x509_tlsfeatures_add(gnutls_x509_tlsfeatures_t f, unsigned int feature)
{
	if (f == NULL) {
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	if (feature > UINT16_MAX)
		return gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);

	if (f->size >= sizeof(f->feature)/sizeof(f->feature[0]))
		return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

	f->feature[f->size++] = feature;

	return 0;
}