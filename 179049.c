*/
PHP_FUNCTION(date_timezone_get)
{
	zval             *object;
	php_date_obj     *dateobj;
	php_timezone_obj *tzobj;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, date_ce_interface) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	if (dateobj->time->is_localtime/* && dateobj->time->tz_info*/) {
		php_date_instantiate(date_ce_timezone, return_value TSRMLS_CC);
		tzobj = (php_timezone_obj *) zend_object_store_get_object(return_value TSRMLS_CC);
		set_timezone_from_timelib_time(tzobj, dateobj->time);
	} else {
		RETURN_FALSE;
	}