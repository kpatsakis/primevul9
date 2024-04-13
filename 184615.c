static int php_plain_files_url_stater(php_stream_wrapper *wrapper, const char *url, int flags, php_stream_statbuf *ssb, php_stream_context *context)
{
	if (strncasecmp(url, "file://", sizeof("file://") - 1) == 0) {
		url += sizeof("file://") - 1;
	}

	if (php_check_open_basedir_ex(url, (flags & PHP_STREAM_URL_STAT_QUIET) ? 0 : 1)) {
		return -1;
	}

#ifdef PHP_WIN32
	if (flags & PHP_STREAM_URL_STAT_LINK) {
		return VCWD_LSTAT(url, &ssb->sb);
	}
#else
# ifdef HAVE_SYMLINK
	if (flags & PHP_STREAM_URL_STAT_LINK) {
		return VCWD_LSTAT(url, &ssb->sb);
	} else
# endif
#endif
		return VCWD_STAT(url, &ssb->sb);
}