SPL_METHOD(DirectoryIterator, getFilename)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_STRING(intern->u.dir.entry.d_name, 1);
}