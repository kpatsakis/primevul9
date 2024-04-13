*/
PHP_METHOD(DateTimeImmutable, setDate)
{
	zval *object, *new_object;
	long  y, m, d;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Olll", &object, date_ce_immutable, &y, &m, &d) == FAILURE) {
		RETURN_FALSE;
	}

	new_object = date_clone_immutable(object TSRMLS_CC);
	php_date_date_set(new_object, y, m, d, return_value TSRMLS_CC);

	RETURN_ZVAL(new_object, 0, 1);