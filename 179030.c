*/
PHP_METHOD(DateTimeImmutable, setTimezone)
{
	zval *object, *new_object;
	zval *timezone_object;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO", &object, date_ce_immutable, &timezone_object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}

	new_object = date_clone_immutable(object TSRMLS_CC);
	php_date_timezone_set(new_object, timezone_object, return_value TSRMLS_CC);

	RETURN_ZVAL(new_object, 0, 1);