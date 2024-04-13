int phar_open_or_create_zip(char *fname, int fname_len, char *alias, int alias_len, int is_data, int options, phar_archive_data** pphar, char **error TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	int ret = phar_create_or_parse_filename(fname, fname_len, alias, alias_len, is_data, options, &phar, error TSRMLS_CC);

	if (FAILURE == ret) {
		return FAILURE;
	}

	if (pphar) {
		*pphar = phar;
	}

	phar->is_data = is_data;

	if (phar->is_zip) {
		return ret;
	}

	if (phar->is_brandnew) {
		phar->internal_file_start = 0;
		phar->is_zip = 1;
		phar->is_tar = 0;
		return SUCCESS;
	}

	/* we've reached here - the phar exists and is a regular phar */
	if (error) {
		spprintf(error, 4096, "phar zip error: phar \"%s\" already exists as a regular phar and must be deleted from disk prior to creating as a zip-based phar", fname);
	}

	return FAILURE;
}