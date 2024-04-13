*/
PHP_METHOD(DateTimeImmutable, setTime)
{
	zval *object, *new_object;
	long  h, i, s = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oll|l", &object, date_ce_immutable, &h, &i, &s) == FAILURE) {
		RETURN_FALSE;
	}

	new_object = date_clone_immutable(object TSRMLS_CC);
	php_date_time_set(new_object, h, i, s, return_value TSRMLS_CC);

	RETURN_ZVAL(new_object, 0, 1);