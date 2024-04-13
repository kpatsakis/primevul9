*/
PHP_FUNCTION(date_time_set)
{
	zval *object;
	long  h, i, s = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oll|l", &object, date_ce_date, &h, &i, &s) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_time_set(object, h, i, s, return_value TSRMLS_CC);

	RETURN_ZVAL(object, 1, 0);