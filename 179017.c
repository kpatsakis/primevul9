*/
PHP_FUNCTION(date_timestamp_set)
{
	zval *object;
	long  timestamp;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &object, date_ce_date, &timestamp) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_timestamp_set(object, timestamp, return_value TSRMLS_CC);

	RETURN_ZVAL(object, 1, 0);