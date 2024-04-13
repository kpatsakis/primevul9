*/
PHP_FUNCTION(date_isodate_set)
{
	zval *object;
	long  y, w, d = 1;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oll|l", &object, date_ce_date, &y, &w, &d) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_isodate_set(object, y, w, d, return_value TSRMLS_CC);

	RETURN_ZVAL(object, 1, 0);