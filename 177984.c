static void phar_copy_cached_phar(phar_archive_data **pphar) /* {{{ */
{
	phar_archive_data *phar;
	HashTable newmanifest;
	char *fname;
	phar_archive_object *objphar;

	phar = (phar_archive_data *) emalloc(sizeof(phar_archive_data));
	*phar = **pphar;
	phar->is_persistent = 0;
	fname = phar->fname;
	phar->fname = estrndup(phar->fname, phar->fname_len);
	phar->ext = phar->fname + (phar->ext - fname);

	if (phar->alias) {
		phar->alias = estrndup(phar->alias, phar->alias_len);
	}

	if (phar->signature) {
		phar->signature = estrdup(phar->signature);
	}

	if (Z_TYPE(phar->metadata) != IS_UNDEF) {
		/* assume success, we would have failed before */
		if (phar->metadata_len) {
			char *buf = estrndup((char *) Z_PTR(phar->metadata), phar->metadata_len);
			phar_parse_metadata(&buf, &phar->metadata, phar->metadata_len);
			efree(buf);
		} else {
			zval_copy_ctor(&phar->metadata);
		}
	}

	zend_hash_init(&newmanifest, sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest_entry, 0);
	zend_hash_copy(&newmanifest, &(*pphar)->manifest, phar_manifest_copy_ctor);
	zend_hash_apply_with_argument(&newmanifest, phar_update_cached_entry, (void *)phar);
	phar->manifest = newmanifest;
	zend_hash_init(&phar->mounted_dirs, sizeof(char *),
		zend_get_hash_value, NULL, 0);
	zend_hash_init(&phar->virtual_dirs, sizeof(char *),
		zend_get_hash_value, NULL, 0);
	zend_hash_copy(&phar->virtual_dirs, &(*pphar)->virtual_dirs, NULL);
	*pphar = phar;

	/* now, scan the list of persistent Phar objects referencing this phar and update the pointers */
	ZEND_HASH_FOREACH_PTR(&PHAR_G(phar_persist_map), objphar) {
		if (objphar->archive->fname_len == phar->fname_len && !memcmp(objphar->archive->fname, phar->fname, phar->fname_len)) {
			objphar->archive = phar;
		}
	} ZEND_HASH_FOREACH_END();
}