PHP_FUNCTION(deg2rad)
{
	double deg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &deg) == FAILURE) {
		return;
	}
	RETURN_DOUBLE((deg / 180.0) * M_PI);
}