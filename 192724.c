PHP_FUNCTION(is_nan)
{
	double dval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &dval) == FAILURE) {
		return;
	}
	RETURN_BOOL(zend_isnan(dval));
}