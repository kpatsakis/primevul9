PHP_FUNCTION(rad2deg)
{
	double rad;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &rad) == FAILURE) {
		return;
	}
	RETURN_DOUBLE((rad / M_PI) * 180);
}