*/
PHP_METHOD(DateTimeImmutable, modify)
{
	zval *object, *new_object;
	char *modify;
	int   modify_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &object, date_ce_immutable, &modify, &modify_len) == FAILURE) {
		RETURN_FALSE;
	}
	
	new_object = date_clone_immutable(object TSRMLS_CC);
	if (php_date_modify(new_object, modify, modify_len TSRMLS_CC)) {
		RETURN_ZVAL(new_object, 0, 1);
	}

	RETURN_FALSE;