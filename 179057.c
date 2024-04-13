*/
PHP_FUNCTION(date_sub)
{
	zval *object, *interval;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO", &object, date_ce_date, &interval, date_ce_interval) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_sub(object, interval, return_value TSRMLS_CC);

	RETURN_ZVAL(object, 1, 0);