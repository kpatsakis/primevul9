*/
PHP_FUNCTION(date_modify)
{
	zval         *object;
	char         *modify;
	int           modify_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &object, date_ce_date, &modify, &modify_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (php_date_modify(object, modify, modify_len TSRMLS_CC)) {
		RETURN_ZVAL(object, 1, 0);
	}

	RETURN_FALSE;