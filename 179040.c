*/
PHP_FUNCTION(date_diff)
{
	zval         *object1, *object2;
	php_date_obj *dateobj1, *dateobj2;
	php_interval_obj *interval;
	long          absolute = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO|l", &object1, date_ce_interface, &object2, date_ce_interface, &absolute) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj1 = (php_date_obj *) zend_object_store_get_object(object1 TSRMLS_CC);
	dateobj2 = (php_date_obj *) zend_object_store_get_object(object2 TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj1->time, DateTimeInterface);
	DATE_CHECK_INITIALIZED(dateobj2->time, DateTimeInterface);
	timelib_update_ts(dateobj1->time, NULL);
	timelib_update_ts(dateobj2->time, NULL);

	php_date_instantiate(date_ce_interval, return_value TSRMLS_CC);
	interval = zend_object_store_get_object(return_value TSRMLS_CC);
	interval->diff = timelib_diff(dateobj1->time, dateobj2->time);
	if (absolute) {
		interval->diff->invert = 0;
	}
	interval->initialized = 1;