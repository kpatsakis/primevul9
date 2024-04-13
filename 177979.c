int phar_copy_on_write(phar_archive_data **pphar) /* {{{ */
{
	zval zv, *pzv;
	phar_archive_data *newpphar;

	ZVAL_PTR(&zv, *pphar);
	if (NULL == (pzv = zend_hash_str_add(&(PHAR_G(phar_fname_map)), (*pphar)->fname, (*pphar)->fname_len, &zv))) {
		return FAILURE;
	}

	phar_copy_cached_phar((phar_archive_data **)&Z_PTR_P(pzv));
	newpphar = Z_PTR_P(pzv);
	/* invalidate phar cache */
	PHAR_G(last_phar) = NULL;
	PHAR_G(last_phar_name) = PHAR_G(last_alias) = NULL;

	if (newpphar->alias_len && NULL == zend_hash_str_add_ptr(&(PHAR_G(phar_alias_map)), newpphar->alias, newpphar->alias_len, newpphar)) {
		zend_hash_str_del(&(PHAR_G(phar_fname_map)), (*pphar)->fname, (*pphar)->fname_len);
		return FAILURE;
	}

	*pphar = newpphar;
	return SUCCESS;
}