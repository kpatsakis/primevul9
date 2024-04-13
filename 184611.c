PHPAPI php_stream *_php_stream_fopen(const char *filename, const char *mode, zend_string **opened_path, int options STREAMS_DC)
{
	char realpath[MAXPATHLEN];
	int open_flags;
	int fd;
	php_stream *ret;
	int persistent = options & STREAM_OPEN_PERSISTENT;
	char *persistent_id = NULL;

	if (FAILURE == php_stream_parse_fopen_modes(mode, &open_flags)) {
		if (options & REPORT_ERRORS) {
			php_error_docref(NULL, E_WARNING, "`%s' is not a valid mode for fopen", mode);
		}
		return NULL;
	}

	if (options & STREAM_ASSUME_REALPATH) {
		strlcpy(realpath, filename, sizeof(realpath));
	} else {
		if (expand_filepath(filename, realpath) == NULL) {
			return NULL;
		}
	}

	if (persistent) {
		spprintf(&persistent_id, 0, "streams_stdio_%d_%s", open_flags, realpath);
		switch (php_stream_from_persistent_id(persistent_id, &ret)) {
			case PHP_STREAM_PERSISTENT_SUCCESS:
				if (opened_path) {
					//TODO: avoid reallocation???
					*opened_path = zend_string_init(realpath, strlen(realpath), 0);
				}
				/* fall through */

			case PHP_STREAM_PERSISTENT_FAILURE:
				efree(persistent_id);;
				return ret;
		}
	}
#ifdef PHP_WIN32
	fd = php_win32_ioutil_open(realpath, open_flags, 0666);
#else
	fd = open(realpath, open_flags, 0666);
#endif
	if (fd != -1)	{

		if (options & STREAM_OPEN_FOR_INCLUDE) {
			ret = php_stream_fopen_from_fd_int_rel(fd, mode, persistent_id);
		} else {
			ret = php_stream_fopen_from_fd_rel(fd, mode, persistent_id);
		}

		if (ret)	{
			if (opened_path) {
				*opened_path = zend_string_init(realpath, strlen(realpath), 0);
			}
			if (persistent_id) {
				efree(persistent_id);
			}

			/* WIN32 always set ISREG flag */
#ifndef PHP_WIN32
			/* sanity checks for include/require.
			 * We check these after opening the stream, so that we save
			 * on fstat() syscalls */
			if (options & STREAM_OPEN_FOR_INCLUDE) {
				php_stdio_stream_data *self = (php_stdio_stream_data*)ret->abstract;
				int r;

				r = do_fstat(self, 0);
				if ((r == 0 && !S_ISREG(self->sb.st_mode))) {
					if (opened_path) {
						zend_string_release(*opened_path);
						*opened_path = NULL;
					}
					php_stream_close(ret);
					return NULL;
				}
			}

			if (options & STREAM_USE_BLOCKING_PIPE) {
				php_stdio_stream_data *self = (php_stdio_stream_data*)ret->abstract;
				self->is_pipe_blocking = 1;
			}
#endif

			return ret;
		}
		close(fd);
	}
	if (persistent_id) {
		efree(persistent_id);
	}
	return NULL;
}