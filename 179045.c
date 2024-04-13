*/
PHP_FUNCTION(date_create_immutable_from_format)
{
	zval           *timezone_object = NULL;
	char           *time_str = NULL, *format_str = NULL;
	int             time_str_len = 0, format_str_len = 0;
	zval            datetime_object;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|O", &format_str, &format_str_len, &time_str, &time_str_len, &timezone_object, date_ce_timezone) == FAILURE) {
		RETURN_FALSE;
	}

	php_date_instantiate(date_ce_immutable, &datetime_object TSRMLS_CC);
	if (!php_date_initialize(zend_object_store_get_object(&datetime_object TSRMLS_CC), time_str, time_str_len, format_str, timezone_object, 0 TSRMLS_CC)) {
		zval_dtor(&datetime_object);
		RETURN_FALSE;
	} else {
		zval *datetime_object_ptr = &datetime_object;
		RETVAL_ZVAL(datetime_object_ptr, 0, 0);
	}