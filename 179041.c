*/
PHP_FUNCTION(date_format)
{
	zval         *object;
	php_date_obj *dateobj;
	char         *format;
	int           format_len;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &object, date_ce_interface, &format, &format_len) == FAILURE) {
		RETURN_FALSE;
	}
	dateobj = (php_date_obj *) zend_object_store_get_object(object TSRMLS_CC);
	DATE_CHECK_INITIALIZED(dateobj->time, DateTime);
	RETURN_STRING(date_format(format, format_len, dateobj->time, dateobj->time->is_localtime), 0);