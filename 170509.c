static int phar_tar_process_metadata(phar_entry_info *entry, php_stream *fp) /* {{{ */
{
	char *metadata;
	size_t save = php_stream_tell(fp), read;
	phar_entry_info *mentry;

	metadata = (char *) safe_emalloc(1, entry->uncompressed_filesize, 1);

	read = php_stream_read(fp, metadata, entry->uncompressed_filesize);
	if (read != entry->uncompressed_filesize) {
		efree(metadata);
		php_stream_seek(fp, save, SEEK_SET);
		return FAILURE;
	}

	if (phar_parse_metadata(&metadata, &entry->metadata, entry->uncompressed_filesize) == FAILURE) {
		/* if not valid serialized data, it is a regular string */
		efree(metadata);
		php_stream_seek(fp, save, SEEK_SET);
		return FAILURE;
	}

	if (entry->filename_len == sizeof(".phar/.metadata.bin")-1 && !memcmp(entry->filename, ".phar/.metadata.bin", sizeof(".phar/.metadata.bin")-1)) {
		entry->phar->metadata = entry->metadata;
		ZVAL_UNDEF(&entry->metadata);
	} else if (entry->filename_len >= sizeof(".phar/.metadata/") + sizeof("/.metadata.bin") - 1 && NULL != (mentry = zend_hash_str_find_ptr(&(entry->phar->manifest), entry->filename + sizeof(".phar/.metadata/") - 1, entry->filename_len - (sizeof("/.metadata.bin") - 1 + sizeof(".phar/.metadata/") - 1)))) {
		/* transfer this metadata to the entry it refers */
		mentry->metadata = entry->metadata;
		ZVAL_UNDEF(&entry->metadata);
	}

	efree(metadata);
	php_stream_seek(fp, save, SEEK_SET);
	return SUCCESS;
}