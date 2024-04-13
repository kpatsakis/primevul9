PHP_FUNCTION(base_convert)
{
	zval **number, temp;
	long frombase, tobase;
	char *result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zll", &number, &frombase, &tobase) == FAILURE) {
		return;
	}
	convert_to_string_ex(number);
	
	if (frombase < 2 || frombase > 36) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid `from base' (%ld)", frombase);
		RETURN_FALSE;
	}
	if (tobase < 2 || tobase > 36) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid `to base' (%ld)", tobase);
		RETURN_FALSE;
	}

	if(_php_math_basetozval(*number, frombase, &temp) == FAILURE) {
		RETURN_FALSE;
	}
	result = _php_math_zvaltobase(&temp, tobase TSRMLS_CC);
	RETVAL_STRING(result, 0);
} 