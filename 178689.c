SPL_METHOD(SplFileObject, fwrite)
{
	spl_filesystem_object *intern = (spl_filesystem_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	char *str;
	int str_len;
	long length = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &str_len, &length) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() > 1) {
		str_len = MAX(0, MIN(length, str_len));
	}
	if (!str_len) {
		RETURN_LONG(0);
	}

	RETURN_LONG(php_stream_write(intern->u.file.stream, str, str_len));
} /* }}} */