
static void php_date_date_set(zval *object, long y, long m, long d, zval *return_value TSRMLS_DC)
{
	php_date_obj *dateobj;

	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	dateobj->time->y = y;
	dateobj->time->m = m;
	dateobj->time->d = d;
	timelib_update_ts(dateobj->time, NULL);