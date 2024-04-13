int gnutls_x509_aia_set(gnutls_x509_aia_t aia,
			const char *oid,
			unsigned san_type,
			const gnutls_datum_t * san)
{
	int ret;
	void *tmp;
	unsigned indx;

	tmp = gnutls_realloc(aia->aia, (aia->size + 1) * sizeof(aia->aia[0]));
	if (tmp == NULL) {
		return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);
	}
	aia->aia = tmp;
	indx = aia->size;
	
	aia->aia[indx].san_type = san_type;
	if (oid) {
		aia->aia[indx].oid.data = (void*)gnutls_strdup(oid);
		aia->aia[indx].oid.size = strlen(oid);
	} else {
		aia->aia[indx].oid.data = NULL;
		aia->aia[indx].oid.size = 0;
	}

	ret = _gnutls_alt_name_process(&aia->aia[indx].san, san_type, san, 0);
	if (ret < 0)
		return gnutls_assert_val(ret);

	aia->size++;

	return 0;	
}