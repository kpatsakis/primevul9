*/
PHP_METHOD(DateTimeImmutable, sub)
{
	zval *object, *interval, *new_object;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO", &object, date_ce_immutable, &interval, date_ce_interval) == FAILURE) {
		RETURN_FALSE;
	}

	new_object = date_clone_immutable(object TSRMLS_CC);
	php_date_sub(new_object, interval, return_value TSRMLS_CC);

	RETURN_ZVAL(new_object, 0, 1);