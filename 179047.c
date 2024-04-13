
static void php_date_timestamp_set(zval *object, long timestamp, zval *return_value TSRMLS_DC)
{
	php_date_obj *dateobj;

	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	timelib_unixtime2local(dateobj->time, (timelib_sll)timestamp);
	timelib_update_ts(dateobj->time, NULL);