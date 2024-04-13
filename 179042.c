*/
PHP_FUNCTION(date_date_set)
{
	zval *object;
	long  y, m, d;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Olll", &object, date_ce_date, &y, &m, &d) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_date_set(object, y, m, d, return_value TSRMLS_CC);

	RETURN_ZVAL(object, 1, 0);