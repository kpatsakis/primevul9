int gnutls_subject_alt_names_set(gnutls_subject_alt_names_t sans,
				 unsigned int san_type,
				 const gnutls_datum_t * san,
				 const char *othername_oid)
{
	int ret;
	gnutls_datum_t copy;
	char *ooc;

	ret = _gnutls_set_strdatum(&copy, san->data, san->size);
	if (ret < 0)
		return gnutls_assert_val(ret);

	if (othername_oid != NULL)
		ooc = gnutls_strdup(othername_oid);
	else
		ooc = NULL;
	ret = subject_alt_names_set(&sans->names, &sans->size,
				    san_type, &copy, ooc, 0);
	if (ret < 0) {
		gnutls_free(copy.data);
		return gnutls_assert_val(ret);
	}

	return 0;
}