static php_stream *php_plain_files_dir_opener(php_stream_wrapper *wrapper, const char *path, const char *mode,
		int options, zend_string **opened_path, php_stream_context *context STREAMS_DC)
{
	DIR *dir = NULL;
	php_stream *stream = NULL;

#ifdef HAVE_GLOB
	if (options & STREAM_USE_GLOB_DIR_OPEN) {
		return php_glob_stream_wrapper.wops->dir_opener(&php_glob_stream_wrapper, path, mode, options, opened_path, context STREAMS_REL_CC);
	}
#endif

	if (((options & STREAM_DISABLE_OPEN_BASEDIR) == 0) && php_check_open_basedir(path)) {
		return NULL;
	}

	dir = VCWD_OPENDIR(path);

#ifdef PHP_WIN32
	if (!dir) {
		php_win32_docref2_from_error(GetLastError(), path, path);
	}

	if (dir && dir->finished) {
		closedir(dir);
		dir = NULL;
	}
#endif
	if (dir) {
		stream = php_stream_alloc(&php_plain_files_dirstream_ops, dir, 0, mode);
		if (stream == NULL)
			closedir(dir);
	}

	return stream;
}