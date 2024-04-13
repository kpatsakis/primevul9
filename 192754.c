PHP_FUNCTION(is_infinite)
{
	double dval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &dval) == FAILURE) {
		return;
	}
	RETURN_BOOL(zend_isinf(dval));
}