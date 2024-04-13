PHP_FUNCTION(pow)
{
	zval *zbase, *zexp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z/z/", &zbase, &zexp) == FAILURE) {
		return;
	}

	pow_function(return_value, zbase, zexp TSRMLS_CC);
}