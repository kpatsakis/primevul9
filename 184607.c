static int php_plain_files_unlink(php_stream_wrapper *wrapper, const char *url, int options, php_stream_context *context)
{
	int ret;

	if (strncasecmp(url, "file://", sizeof("file://") - 1) == 0) {
		url += sizeof("file://") - 1;
	}

	if (php_check_open_basedir(url)) {
		return 0;
	}

	ret = VCWD_UNLINK(url);
	if (ret == -1) {
		if (options & REPORT_ERRORS) {
			php_error_docref1(NULL, url, E_WARNING, "%s", strerror(errno));
		}
		return 0;
	}

	/* Clear stat cache (and realpath cache) */
	php_clear_stat_cache(1, NULL, 0);

	return 1;
}