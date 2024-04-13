
static void php_date_time_set(zval *object, long h, long i, long s, zval *return_value TSRMLS_DC)
{
	php_date_obj *dateobj;

	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	dateobj->time->h = h;
	dateobj->time->i = i;
	dateobj->time->s = s;
	timelib_update_ts(dateobj->time, NULL);