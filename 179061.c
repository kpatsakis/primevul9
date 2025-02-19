 *  */
PHP_METHOD(DateTimeZone, __wakeup)
{
	zval             *object = getThis();
	php_timezone_obj *tzobj;
	HashTable        *myht;

	tzobj = (php_timezone_obj *) zend_object_store_get_object(object TSRMLS_CC);

	myht = Z_OBJPROP_P(object);
	
	if(php_date_timezone_initialize_from_hash(&return_value, &tzobj, myht TSRMLS_CC) != SUCCESS) {
		php_error_docref(NULL, E_ERROR, "Timezone initialization failed");
	}