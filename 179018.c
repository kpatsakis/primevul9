*/
PHP_METHOD(DateTimeImmutable, __set_state)
{
	php_date_obj     *dateobj;
	zval             *array;
	HashTable        *myht;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &array) == FAILURE) {
		RETURN_FALSE;
	}

	myht = HASH_OF(array);

	php_date_instantiate(date_ce_immutable, return_value TSRMLS_CC);
	dateobj = (php_date_obj *) zend_object_store_get_object(return_value TSRMLS_CC);
	if (!php_date_initialize_from_hash(&dateobj, myht TSRMLS_CC)) {
		php_error(E_ERROR, "Invalid serialization data for DateTimeImmutable object");
	}