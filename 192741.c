PHP_FUNCTION(octdec)
{
	zval **arg;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		return;
	}
	convert_to_string_ex(arg);
	if (_php_math_basetozval(*arg, 8, return_value) == FAILURE) {
		RETURN_FALSE;
	}
}