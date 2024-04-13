int phar_create_writeable_entry(phar_archive_data *phar, phar_entry_info *entry, char **error) /* {{{ */
{
	if (entry->fp_type == PHAR_MOD) {
		/* already newly created, truncate */
		php_stream_truncate_set_size(entry->fp, 0);

		entry->old_flags = entry->flags;
		entry->is_modified = 1;
		phar->is_modified = 1;
		/* reset file size */
		entry->uncompressed_filesize = 0;
		entry->compressed_filesize = 0;
		entry->crc32 = 0;
		entry->flags = PHAR_ENT_PERM_DEF_FILE;
		entry->fp_type = PHAR_MOD;
		entry->offset = 0;
		return SUCCESS;
	}

	if (error) {
		*error = NULL;
	}

	/* open a new temp file for writing */
	if (entry->link) {
		efree(entry->link);
		entry->link = NULL;
		entry->tar_type = (entry->is_tar ? TAR_FILE : '\0');
	}

	entry->fp = php_stream_fopen_tmpfile();

	if (!entry->fp) {
		if (error) {
			spprintf(error, 0, "phar error: unable to create temporary file");
		}
		return FAILURE;
	}

	entry->old_flags = entry->flags;
	entry->is_modified = 1;
	phar->is_modified = 1;
	/* reset file size */
	entry->uncompressed_filesize = 0;
	entry->compressed_filesize = 0;
	entry->crc32 = 0;
	entry->flags = PHAR_ENT_PERM_DEF_FILE;
	entry->fp_type = PHAR_MOD;
	entry->offset = 0;
	return SUCCESS;
}