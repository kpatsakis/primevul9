PHP_FUNCTION(atan2)
{
	double num1, num2;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dd", &num1, &num2) == FAILURE) {
		return;
	}
	RETURN_DOUBLE(atan2(num1, num2));
}