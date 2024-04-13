*/
PHP_FUNCTION(date_offset_get)
{
	zval                *object;
	php_date_obj        *dateobj;
	timelib_time_offset *offset;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, date_ce_interface) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	if (dateobj->time->is_localtime/* && dateobj->time->tz_info*/) {
		switch (dateobj->time->zone_type) {
			case TIMELIB_ZONETYPE_ID:
				offset = timelib_get_time_zone_info(dateobj->time->sse, dateobj->time->tz_info);
				RETVAL_LONG(offset->offset);
				timelib_time_offset_dtor(offset);
				break;
			case TIMELIB_ZONETYPE_OFFSET:
				RETVAL_LONG(dateobj->time->z * -60);
				break;
			case TIMELIB_ZONETYPE_ABBR:
				RETVAL_LONG((dateobj->time->z - (60 * dateobj->time->dst)) * -60);
				break;
		}
		return;
	} else {
		RETURN_LONG(0);
	}