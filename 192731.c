PHP_FUNCTION(atan)
{
	double num;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &num) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(atan(num));
}