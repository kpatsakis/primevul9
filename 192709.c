PHP_FUNCTION(acosh)
{
	double num;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(php_acosh(num));
}