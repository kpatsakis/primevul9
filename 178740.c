void spl_filesystem_object_construct(INTERNAL_FUNCTION_PARAMETERS, long ctor_flags) /* {{{ */
{
	spl_filesystem_object *intern;
	char *path;
	int parsed, len;
	long flags;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, spl_ce_UnexpectedValueException, &error_handling TSRMLS_CC);

	if (SPL_HAS_FLAG(ctor_flags, DIT_CTOR_FLAGS)) {
		flags = SPL_FILE_DIR_KEY_AS_PATHNAME|SPL_FILE_DIR_CURRENT_AS_FILEINFO;
		parsed = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &path, &len, &flags);
	} else {
		flags = SPL_FILE_DIR_KEY_AS_PATHNAME|SPL_FILE_DIR_CURRENT_AS_SELF;
		parsed = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &path, &len);
	}
	if (SPL_HAS_FLAG(ctor_flags, SPL_FILE_DIR_SKIPDOTS)) {
		flags |= SPL_FILE_DIR_SKIPDOTS;
	}
	if (SPL_HAS_FLAG(ctor_flags, SPL_FILE_DIR_UNIXPATHS)) {
		flags |= SPL_FILE_DIR_UNIXPATHS;
	}
	if (parsed == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	if (!len) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Directory name must not be empty.");
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	if (intern->_path) {
		/* object is alreay initialized */
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Directory object is already initialized");
		return;
	}
	intern->flags = flags;
#ifdef HAVE_GLOB
	if (SPL_HAS_FLAG(ctor_flags, DIT_CTOR_GLOB) && strstr(path, "glob://") != path) {
		spprintf(&path, 0, "glob://%s", path);
		spl_filesystem_dir_open(intern, path TSRMLS_CC);
		efree(path);
	} else
#endif
	{
		spl_filesystem_dir_open(intern, path TSRMLS_CC);

	}

	intern->u.dir.is_recursive = instanceof_function(intern->std.ce, spl_ce_RecursiveDirectoryIterator TSRMLS_CC) ? 1 : 0;

	zend_restore_error_handling(&error_handling TSRMLS_CC);
}