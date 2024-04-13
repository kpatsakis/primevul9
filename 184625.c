static int php_plain_files_metadata(php_stream_wrapper *wrapper, const char *url, int option, void *value, php_stream_context *context)
{
	struct utimbuf *newtime;
#if !defined(WINDOWS) && !defined(NETWARE)
	uid_t uid;
	gid_t gid;
#endif
	mode_t mode;
	int ret = 0;
#ifdef PHP_WIN32
	int url_len = (int)strlen(url);
#endif

#ifdef PHP_WIN32
	if (!php_win32_check_trailing_space(url, url_len)) {
		php_error_docref1(NULL, url, E_WARNING, "%s", strerror(ENOENT));
		return 0;
	}
#endif

	if (strncasecmp(url, "file://", sizeof("file://") - 1) == 0) {
		url += sizeof("file://") - 1;
	}

	if (php_check_open_basedir(url)) {
		return 0;
	}

	switch(option) {
		case PHP_STREAM_META_TOUCH:
			newtime = (struct utimbuf *)value;
			if (VCWD_ACCESS(url, F_OK) != 0) {
				FILE *file = VCWD_FOPEN(url, "w");
				if (file == NULL) {
					php_error_docref1(NULL, url, E_WARNING, "Unable to create file %s because %s", url, strerror(errno));
					return 0;
				}
				fclose(file);
			}

			ret = VCWD_UTIME(url, newtime);
			break;
#if !defined(WINDOWS) && !defined(NETWARE)
		case PHP_STREAM_META_OWNER_NAME:
		case PHP_STREAM_META_OWNER:
			if(option == PHP_STREAM_META_OWNER_NAME) {
				if(php_get_uid_by_name((char *)value, &uid) != SUCCESS) {
					php_error_docref1(NULL, url, E_WARNING, "Unable to find uid for %s", (char *)value);
					return 0;
				}
			} else {
				uid = (uid_t)*(long *)value;
			}
			ret = VCWD_CHOWN(url, uid, -1);
			break;
		case PHP_STREAM_META_GROUP:
		case PHP_STREAM_META_GROUP_NAME:
			if(option == PHP_STREAM_META_GROUP_NAME) {
				if(php_get_gid_by_name((char *)value, &gid) != SUCCESS) {
					php_error_docref1(NULL, url, E_WARNING, "Unable to find gid for %s", (char *)value);
					return 0;
				}
			} else {
				gid = (gid_t)*(long *)value;
			}
			ret = VCWD_CHOWN(url, -1, gid);
			break;
#endif
		case PHP_STREAM_META_ACCESS:
			mode = (mode_t)*(zend_long *)value;
			ret = VCWD_CHMOD(url, mode);
			break;
		default:
			php_error_docref1(NULL, url, E_WARNING, "Unknown option %d for stream_metadata", option);
			return 0;
	}
	if (ret == -1) {
		php_error_docref1(NULL, url, E_WARNING, "Operation failed: %s", strerror(errno));
		return 0;
	}
	php_clear_stat_cache(0, NULL, 0);
	return 1;
}