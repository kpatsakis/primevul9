int subject_alt_names_set(struct name_st **names,
			  unsigned int *size,
			  unsigned int san_type,
			  gnutls_datum_t * san, char *othername_oid,
			  unsigned raw)
{
	void *tmp;
	int ret;

	tmp = gnutls_realloc(*names, (*size + 1) * sizeof((*names)[0]));
	if (tmp == NULL) {
		return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);
	}
	*names = tmp;

	ret = _gnutls_alt_name_assign_virt_type(&(*names)[*size], san_type, san, othername_oid, raw);
	if (ret < 0)
		return gnutls_assert_val(ret);

	(*size)++;
	return 0;
}