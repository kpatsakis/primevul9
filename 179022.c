 *  */
PHP_METHOD(DateTimeZone, __set_state)
{
	php_timezone_obj *tzobj;
	zval             *array;
	HashTable        *myht;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &array) == FAILURE) {
		RETURN_FALSE;
	}

	myht = HASH_OF(array);

	php_date_instantiate(date_ce_timezone, return_value TSRMLS_CC);
	tzobj = (php_timezone_obj *) zend_object_store_get_object(return_value TSRMLS_CC);
	if(php_date_timezone_initialize_from_hash(&return_value, &tzobj, myht TSRMLS_CC) != SUCCESS) {
		php_error_docref(NULL, E_ERROR, "Timezone initialization failed");
	}