*/
PHP_FUNCTION(date_timezone_set)
{
	zval *object;
	zval *timezone_object;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO", &object, date_ce_date, &timezone_object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_timezone_set(object, timezone_object, return_value TSRMLS_CC);

	RETURN_ZVAL(object, 1, 0);