int phar_tar_setmetadata(zval *metadata, phar_entry_info *entry, char **error) /* {{{ */
{
	php_serialize_data_t metadata_hash;

	if (entry->metadata_str.s) {
		smart_str_free(&entry->metadata_str);
	}

	entry->metadata_str.s = NULL;
	PHP_VAR_SERIALIZE_INIT(metadata_hash);
	php_var_serialize(&entry->metadata_str, metadata, &metadata_hash);
	PHP_VAR_SERIALIZE_DESTROY(metadata_hash);
	entry->uncompressed_filesize = entry->compressed_filesize = entry->metadata_str.s ? ZSTR_LEN(entry->metadata_str.s) : 0;

	if (entry->fp && entry->fp_type == PHAR_MOD) {
		php_stream_close(entry->fp);
	}

	entry->fp_type = PHAR_MOD;
	entry->is_modified = 1;
	entry->fp = php_stream_fopen_tmpfile();
	entry->offset = entry->offset_abs = 0;
	if (entry->fp == NULL) {
		spprintf(error, 0, "phar error: unable to create temporary file");
		return -1;
	}
	if (ZSTR_LEN(entry->metadata_str.s) != php_stream_write(entry->fp, ZSTR_VAL(entry->metadata_str.s), ZSTR_LEN(entry->metadata_str.s))) {
		spprintf(error, 0, "phar tar error: unable to write metadata to magic metadata file \"%s\"", entry->filename);
		zend_hash_str_del(&(entry->phar->manifest), entry->filename, entry->filename_len);
		return ZEND_HASH_APPLY_STOP;
	}

	return ZEND_HASH_APPLY_KEEP;
}