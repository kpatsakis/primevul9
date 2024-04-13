*/
PHP_METHOD(DateTimeImmutable, setTimestamp)
{
	zval *object, *new_object;
	long  timestamp;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ol", &object, date_ce_immutable, &timestamp) == FAILURE) {
		RETURN_FALSE;
	}

	new_object = date_clone_immutable(object TSRMLS_CC);
	php_date_timestamp_set(new_object, timestamp, return_value TSRMLS_CC);

	RETURN_ZVAL(new_object, 0, 1);