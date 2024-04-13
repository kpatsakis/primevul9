PHP_FUNCTION(decbin)
{
	zval **arg;
	char *result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z", &arg) == FAILURE) {
		return;
	}
	convert_to_long_ex(arg);
	result = _php_math_longtobase(*arg, 2);
	RETURN_STRING(result, 0);
}