PHPAPI php_stream *_php_stream_fopen_with_path(const char *filename, const char *mode, const char *path, zend_string **opened_path, int options STREAMS_DC)
{
	/* code ripped off from fopen_wrappers.c */
	char *pathbuf, *end;
	const char *ptr;
	char trypath[MAXPATHLEN];
	php_stream *stream;
	int filename_length;
	zend_string *exec_filename;

	if (opened_path) {
		*opened_path = NULL;
	}

	if(!filename) {
		return NULL;
	}

	filename_length = (int)strlen(filename);
#ifndef PHP_WIN32
	(void) filename_length;
#endif

	/* Relative path open */
	if (*filename == '.' && (IS_SLASH(filename[1]) || filename[1] == '.')) {
		/* further checks, we could have ....... filenames */
		ptr = filename + 1;
		if (*ptr == '.') {
			while (*(++ptr) == '.');
			if (!IS_SLASH(*ptr)) { /* not a relative path after all */
				goto not_relative_path;
			}
		}


		if (((options & STREAM_DISABLE_OPEN_BASEDIR) == 0) && php_check_open_basedir(filename)) {
			return NULL;
		}

		return php_stream_fopen_rel(filename, mode, opened_path, options);
	}

not_relative_path:

	/* Absolute path open */
	if (IS_ABSOLUTE_PATH(filename, filename_length)) {

		if (((options & STREAM_DISABLE_OPEN_BASEDIR) == 0) && php_check_open_basedir(filename)) {
			return NULL;
		}

		return php_stream_fopen_rel(filename, mode, opened_path, options);
	}

#ifdef PHP_WIN32
	if (IS_SLASH(filename[0])) {
		size_t cwd_len;
		char *cwd;
		cwd = virtual_getcwd_ex(&cwd_len);
		/* getcwd() will return always return [DRIVE_LETTER]:/) on windows. */
		*(cwd+3) = '\0';

		if (snprintf(trypath, MAXPATHLEN, "%s%s", cwd, filename) >= MAXPATHLEN) {
			php_error_docref(NULL, E_NOTICE, "%s/%s path was truncated to %d", cwd, filename, MAXPATHLEN);
		}

		efree(cwd);

		if (((options & STREAM_DISABLE_OPEN_BASEDIR) == 0) && php_check_open_basedir(trypath)) {
			return NULL;
		}

		return php_stream_fopen_rel(trypath, mode, opened_path, options);
	}
#endif

	if (!path || !*path) {
		return php_stream_fopen_rel(filename, mode, opened_path, options);
	}

	/* check in provided path */
	/* append the calling scripts' current working directory
	 * as a fall back case
	 */
	if (zend_is_executing() &&
	    (exec_filename = zend_get_executed_filename_ex()) != NULL) {
		const char *exec_fname = ZSTR_VAL(exec_filename);
		size_t exec_fname_length = ZSTR_LEN(exec_filename);

		while ((--exec_fname_length < SIZE_MAX) && !IS_SLASH(exec_fname[exec_fname_length]));
		if (exec_fname_length<=0) {
			/* no path */
			pathbuf = estrdup(path);
		} else {
			size_t path_length = strlen(path);

			pathbuf = (char *) emalloc(exec_fname_length + path_length +1 +1);
			memcpy(pathbuf, path, path_length);
			pathbuf[path_length] = DEFAULT_DIR_SEPARATOR;
			memcpy(pathbuf+path_length+1, exec_fname, exec_fname_length);
			pathbuf[path_length + exec_fname_length +1] = '\0';
		}
	} else {
		pathbuf = estrdup(path);
	}

	ptr = pathbuf;

	while (ptr && *ptr) {
		end = strchr(ptr, DEFAULT_DIR_SEPARATOR);
		if (end != NULL) {
			*end = '\0';
			end++;
		}
		if (*ptr == '\0') {
			goto stream_skip;
		}
		if (snprintf(trypath, MAXPATHLEN, "%s/%s", ptr, filename) >= MAXPATHLEN) {
			php_error_docref(NULL, E_NOTICE, "%s/%s path was truncated to %d", ptr, filename, MAXPATHLEN);
		}

		if (((options & STREAM_DISABLE_OPEN_BASEDIR) == 0) && php_check_open_basedir_ex(trypath, 0)) {
			goto stream_skip;
		}

		stream = php_stream_fopen_rel(trypath, mode, opened_path, options);
		if (stream) {
			efree(pathbuf);
			return stream;
		}
stream_skip:
		ptr = end;
	} /* end provided path */

	efree(pathbuf);
	return NULL;

}