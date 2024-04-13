PHP_FUNCTION(sin)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(sin(num));
}