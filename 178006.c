int phar_separate_entry_fp(phar_entry_info *entry, char **error) /* {{{ */
{
	php_stream *fp;
	phar_entry_info *link;

	if (FAILURE == phar_open_entry_fp(entry, error, 1)) {
		return FAILURE;
	}

	if (entry->fp_type == PHAR_MOD) {
		return SUCCESS;
	}

	fp = php_stream_fopen_tmpfile();
	if (fp == NULL) {
		spprintf(error, 0, "phar error: unable to create temporary file");
		return FAILURE;
	}
	phar_seek_efp(entry, 0, SEEK_SET, 0, 1);
	link = phar_get_link_source(entry);

	if (!link) {
		link = entry;
	}

	if (SUCCESS != php_stream_copy_to_stream_ex(phar_get_efp(link, 0), fp, link->uncompressed_filesize, NULL)) {
		if (error) {
			spprintf(error, 4096, "phar error: cannot separate entry file \"%s\" contents in phar archive \"%s\" for write access", entry->filename, entry->phar->fname);
		}
		return FAILURE;
	}

	if (entry->link) {
		efree(entry->link);
		entry->link = NULL;
		entry->tar_type = (entry->is_tar ? TAR_FILE : '\0');
	}

	entry->offset = 0;
	entry->fp = fp;
	entry->fp_type = PHAR_MOD;
	entry->is_modified = 1;
	return SUCCESS;
}