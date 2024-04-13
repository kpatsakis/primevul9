static php_stream *php_plain_files_stream_opener(php_stream_wrapper *wrapper, const char *path, const char *mode,
		int options, zend_string **opened_path, php_stream_context *context STREAMS_DC)
{
	if (((options & STREAM_DISABLE_OPEN_BASEDIR) == 0) && php_check_open_basedir(path)) {
		return NULL;
	}

	return php_stream_fopen_rel(path, mode, opened_path, options);
}