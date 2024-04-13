static int php_plain_files_rmdir(php_stream_wrapper *wrapper, const char *url, int options, php_stream_context *context)
{
	if (strncasecmp(url, "file://", sizeof("file://") - 1) == 0) {
		url += sizeof("file://") - 1;
	}

	if (php_check_open_basedir(url)) {
		return 0;
	}

#ifdef PHP_WIN32
	if (!php_win32_check_trailing_space(url, (int)strlen(url))) {
		php_error_docref1(NULL, url, E_WARNING, "%s", strerror(ENOENT));
		return 0;
	}
#endif

	if (VCWD_RMDIR(url) < 0) {
		php_error_docref1(NULL, url, E_WARNING, "%s", strerror(errno));
		return 0;
	}

	/* Clear stat cache (and realpath cache) */
	php_clear_stat_cache(1, NULL, 0);

	return 1;
}