int phar_copy_entry_fp(phar_entry_info *source, phar_entry_info *dest, char **error) /* {{{ */
{
	phar_entry_info *link;

	if (FAILURE == phar_open_entry_fp(source, error, 1)) {
		return FAILURE;
	}

	if (dest->link) {
		efree(dest->link);
		dest->link = NULL;
		dest->tar_type = (dest->is_tar ? TAR_FILE : '\0');
	}

	dest->fp_type = PHAR_MOD;
	dest->offset = 0;
	dest->is_modified = 1;
	dest->fp = php_stream_fopen_tmpfile();
	if (dest->fp == NULL) {
		spprintf(error, 0, "phar error: unable to create temporary file");
		return EOF;
	}
	phar_seek_efp(source, 0, SEEK_SET, 0, 1);
	link = phar_get_link_source(source);

	if (!link) {
		link = source;
	}

	if (SUCCESS != php_stream_copy_to_stream_ex(phar_get_efp(link, 0), dest->fp, link->uncompressed_filesize, NULL)) {
		php_stream_close(dest->fp);
		dest->fp_type = PHAR_FP;
		if (error) {
			spprintf(error, 4096, "phar error: unable to copy contents of file \"%s\" to \"%s\" in phar archive \"%s\"", source->filename, dest->filename, source->phar->fname);
		}
		return FAILURE;
	}

	return SUCCESS;
}