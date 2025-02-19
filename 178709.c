SPL_METHOD(SplFileInfo, getFilename)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	int path_len;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	spl_filesystem_object_get_path(intern, &path_len TSRMLS_CC);

	if (path_len && path_len < intern->file_name_len) {
		RETURN_STRINGL(intern->file_name + path_len + 1, intern->file_name_len - (path_len + 1), 1);
	} else {
		RETURN_STRINGL(intern->file_name, intern->file_name_len, 1);
	}
}