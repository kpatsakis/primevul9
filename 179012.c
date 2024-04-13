
static HashTable *date_object_get_properties_timezone(zval *object TSRMLS_DC)
{
	HashTable *props;
	zval *zv;
	php_timezone_obj     *tzobj;


	tzobj = (php_timezone_obj *) zend_object_store_get_object(object TSRMLS_CC);

	props = zend_std_get_properties(object TSRMLS_CC);

	if (!tzobj->initialized) {
		return props;
	}

	MAKE_STD_ZVAL(zv);
	ZVAL_LONG(zv, tzobj->type);
	zend_hash_update(props, "timezone_type", 14, &zv, sizeof(zv), NULL);

	MAKE_STD_ZVAL(zv);
	switch (tzobj->type) {
		case TIMELIB_ZONETYPE_ID:
			ZVAL_STRING(zv, tzobj->tzi.tz->name, 1);
			break;
		case TIMELIB_ZONETYPE_OFFSET: {
			char *tmpstr = emalloc(sizeof("UTC+05:00"));

			snprintf(tmpstr, sizeof("+05:00"), "%c%02d:%02d",
			tzobj->tzi.utc_offset > 0 ? '-' : '+',
			abs(tzobj->tzi.utc_offset / 60),
			abs((tzobj->tzi.utc_offset % 60)));

			ZVAL_STRING(zv, tmpstr, 0);
			}
			break;
		case TIMELIB_ZONETYPE_ABBR:
			ZVAL_STRING(zv, tzobj->tzi.z.abbr, 1);
			break;
	}
	zend_hash_update(props, "timezone", 9, &zv, sizeof(zv), NULL);

	return props;