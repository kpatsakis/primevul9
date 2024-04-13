static void spl_filesystem_dir_open(spl_filesystem_object* intern, char *path TSRMLS_DC)
{
	int skip_dots = SPL_HAS_FLAG(intern->flags, SPL_FILE_DIR_SKIPDOTS);

	intern->type = SPL_FS_DIR;
	intern->_path_len = strlen(path);
	intern->u.dir.dirp = php_stream_opendir(path, REPORT_ERRORS, FG(default_context));

	if (intern->_path_len > 1 && IS_SLASH_AT(path, intern->_path_len-1)) {
		intern->_path = estrndup(path, --intern->_path_len);
	} else {
		intern->_path = estrndup(path, intern->_path_len);
	}
	intern->u.dir.index = 0;

	if (EG(exception) || intern->u.dir.dirp == NULL) {
		intern->u.dir.entry.d_name[0] = '\0';
		if (!EG(exception)) {
			/* open failed w/out notice (turned to exception due to EH_THROW) */
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0
				TSRMLS_CC, "Failed to open directory \"%s\"", path);
		}
	} else {
		do {
			spl_filesystem_dir_read(intern TSRMLS_CC);
		} while (skip_dots && spl_filesystem_is_dot(intern->u.dir.entry.d_name));
	}
}