
static void php_date_isodate_set(zval *object, long y, long w, long d, zval *return_value TSRMLS_DC)
{
	php_date_obj *dateobj;

	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	dateobj->time->y = y;
	dateobj->time->m = 1;
	dateobj->time->d = 1;
	memset(&dateobj->time->relative, 0, sizeof(dateobj->time->relative));
	dateobj->time->relative.d = timelib_daynr_from_weeknr(y, w, d);
	dateobj->time->have_relative = 1;
	
	timelib_update_ts(dateobj->time, NULL);