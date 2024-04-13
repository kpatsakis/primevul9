int phar_open_archive_fp(phar_archive_data *phar) /* {{{ */
{
	if (phar_get_pharfp(phar)) {
		return SUCCESS;
	}

	if (php_check_open_basedir(phar->fname)) {
		return FAILURE;
	}

	phar_set_pharfp(phar, php_stream_open_wrapper(phar->fname, "rb", IGNORE_URL|STREAM_MUST_SEEK|0, NULL));

	if (!phar_get_pharfp(phar)) {
		return FAILURE;
	}

	return SUCCESS;
}