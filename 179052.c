
static void php_date_add(zval *object, zval *interval, zval *return_value TSRMLS_DC)
{
	php_date_obj     *dateobj;
	php_interval_obj *intobj;
	timelib_time     *new_time;

	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	intobj = (php_interval_obj *) zend_object_store_get_object(interval TSRMLS_CC);
	DATE_CHECK_INITIALIZED(intobj->initialized, DateInterval);

	new_time = timelib_add(dateobj->time, intobj->diff);
	timelib_time_dtor(dateobj->time);
	dateobj->time = new_time;