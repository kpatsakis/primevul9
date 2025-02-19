SPL_METHOD(SplFileInfo, getLinkTarget)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	int ret;
	char buff[MAXPATHLEN];
	zend_error_handling error_handling;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_replace_error_handling(EH_THROW, spl_ce_RuntimeException, &error_handling TSRMLS_CC);

#if defined(PHP_WIN32) || HAVE_SYMLINK
	if (intern->file_name == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty filename");
		RETURN_FALSE;
	} else if (!IS_ABSOLUTE_PATH(intern->file_name, intern->file_name_len)) {
		char expanded_path[MAXPATHLEN];
		if (!expand_filepath_with_mode(intern->file_name, expanded_path, NULL, 0, CWD_EXPAND  TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "No such file or directory");
			RETURN_FALSE;
		}
		ret = php_sys_readlink(expanded_path, buff, MAXPATHLEN - 1);
	} else {
		ret = php_sys_readlink(intern->file_name, buff,  MAXPATHLEN-1);
	}
#else
	ret = -1; /* always fail if not implemented */
#endif

	if (ret == -1) {
		zend_throw_exception_ex(spl_ce_RuntimeException, 0 TSRMLS_CC, "Unable to read link %s, error: %s", intern->file_name, strerror(errno));
		RETVAL_FALSE;
	} else {
		/* Append NULL to the end of the string */
		buff[ret] = '\0';

		RETVAL_STRINGL(buff, ret, 1);
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);
}