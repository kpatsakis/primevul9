*/
PHP_FUNCTION(date_timestamp_get)
{
	zval         *object;
	php_date_obj *dateobj;
	long          timestamp;
	int           error;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, date_ce_interface) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	timelib_update_ts(dateobj->time, NULL);

	timestamp = timelib_date_to_int(dateobj->time, &error);
	if (error) {
		RETURN_FALSE;
	} else {
		RETVAL_LONG(timestamp);
	}