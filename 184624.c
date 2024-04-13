static int php_plain_files_mkdir(php_stream_wrapper *wrapper, const char *dir, int mode, int options, php_stream_context *context)
{
	int ret, recursive = options & PHP_STREAM_MKDIR_RECURSIVE;
	char *p;

	if (strncasecmp(dir, "file://", sizeof("file://") - 1) == 0) {
		dir += sizeof("file://") - 1;
	}

	if (!recursive) {
		ret = php_mkdir(dir, mode);
	} else {
		/* we look for directory separator from the end of string, thus hopefuly reducing our work load */
		char *e;
		zend_stat_t sb;
		int dir_len = (int)strlen(dir);
		int offset = 0;
		char buf[MAXPATHLEN];

		if (!expand_filepath_with_mode(dir, buf, NULL, 0, CWD_EXPAND )) {
			php_error_docref(NULL, E_WARNING, "Invalid path");
			return 0;
		}

		e = buf +  strlen(buf);

		if ((p = memchr(buf, DEFAULT_SLASH, dir_len))) {
			offset = p - buf + 1;
		}

		if (p && dir_len == 1) {
			/* buf == "DEFAULT_SLASH" */
		}
		else {
			/* find a top level directory we need to create */
			while ( (p = strrchr(buf + offset, DEFAULT_SLASH)) || (offset != 1 && (p = strrchr(buf, DEFAULT_SLASH))) ) {
				int n = 0;

				*p = '\0';
				while (p > buf && *(p-1) == DEFAULT_SLASH) {
					++n;
					--p;
					*p = '\0';
				}
				if (VCWD_STAT(buf, &sb) == 0) {
					while (1) {
						*p = DEFAULT_SLASH;
						if (!n) break;
						--n;
						++p;
					}
					break;
				}
			}
		}

		if (p == buf) {
			ret = php_mkdir(dir, mode);
		} else if (!(ret = php_mkdir(buf, mode))) {
			if (!p) {
				p = buf;
			}
			/* create any needed directories if the creation of the 1st directory worked */
			while (++p != e) {
				if (*p == '\0') {
					*p = DEFAULT_SLASH;
					if ((*(p+1) != '\0') &&
						(ret = VCWD_MKDIR(buf, (mode_t)mode)) < 0) {
						if (options & REPORT_ERRORS) {
							php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
						}
						break;
					}
				}
			}
		}
	}
	if (ret < 0) {
		/* Failure */
		return 0;
	} else {
		/* Success */
		return 1;
	}
}