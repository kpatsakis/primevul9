
static void php_date_sub(zval *object, zval *interval, zval *return_value TSRMLS_DC)
{
	php_date_obj     *dateobj;
	php_interval_obj *intobj;
	timelib_time     *new_time;

	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	intobj = (php_interval_obj *) zend_object_store_get_object(interval TSRMLS_CC);
	DATE_CHECK_INITIALIZED(intobj->initialized, DateInterval);

	if (intobj->diff->have_special_relative) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Only non-special relative time specifications are supported for subtraction");
		return;
	}

	new_time = timelib_sub(dateobj->time, intobj->diff);
	timelib_time_dtor(dateobj->time);
	dateobj->time = new_time;