*/
PHP_FUNCTION(timezone_offset_get)
{
	zval                *object, *dateobject;
	php_timezone_obj    *tzobj;
	php_date_obj        *dateobj;
	timelib_time_offset *offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO", &object, date_ce_timezone, &dateobject, date_ce_interface) == FAILURE) {
		RETURN_FALSE;
	}
	tzobj = (php_timezone_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(tzobj->initialized, DateTimeZone);
	dateobj = (php_date_obj *) zend_object_store_get_object(dateobject TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTimeInterface);

	switch (tzobj->type) {
		case TIMELIB_ZONETYPE_ID:
			offset = timelib_get_time_zone_info(dateobj->time->sse, tzobj->tzi.tz);
			RETVAL_LONG(offset->offset);
			timelib_time_offset_dtor(offset);
			break;
		case TIMELIB_ZONETYPE_OFFSET:
			RETURN_LONG(tzobj->tzi.utc_offset * -60);
			break;
		case TIMELIB_ZONETYPE_ABBR:
			RETURN_LONG((tzobj->tzi.z.utc_offset - (tzobj->tzi.z.dst*60)) * -60);
			break;
	}