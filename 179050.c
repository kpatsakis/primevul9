
static void php_date_timezone_set(zval *object, zval *timezone_object, zval *return_value TSRMLS_DC)
{
	php_date_obj     *dateobj;
	php_timezone_obj *tzobj;

	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	tzobj = (php_timezone_obj *) zend_object_store_get_object(timezone_object TSRMLS_CC);

	switch (tzobj->type) {
		case TIMELIB_ZONETYPE_OFFSET:
			timelib_set_timezone_from_offset(dateobj->time, tzobj->tzi.utc_offset);
			break;
		case TIMELIB_ZONETYPE_ABBR:
			timelib_set_timezone_from_abbr(dateobj->time, tzobj->tzi.z);
			break;
		case TIMELIB_ZONETYPE_ID:
			timelib_set_timezone(dateobj->time, tzobj->tzi.tz);
			break;
	}
	timelib_unixtime2local(dateobj->time, dateobj->time->sse);