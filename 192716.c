PHP_FUNCTION(log)
{
	double num, base = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d|d", &num, &base) == FAILURE) {
		return;
	}
	if (ZEND_NUM_ARGS() == 1) {
		RETURN_DOUBLE(log(num));
	}
	if (base <= 0.0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "base must be greater than 0");				
		RETURN_FALSE;
	}
	if (base == 1) {
		RETURN_DOUBLE(php_get_nan());
	} else {
		RETURN_DOUBLE(log(num) / log(base));
	}
}