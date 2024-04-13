*/
PHP_METHOD(DateTimeImmutable, setISODate)
{
	zval *object, *new_object;
	long  y, w, d = 1;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oll|l", &object, date_ce_immutable, &y, &w, &d) == FAILURE) {
		RETURN_FALSE;
	}

	new_object = date_clone_immutable(object TSRMLS_CC);
	php_date_isodate_set(new_object, y, w, d, return_value TSRMLS_CC);

	RETURN_ZVAL(new_object, 0, 1);