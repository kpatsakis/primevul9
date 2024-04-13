*/
PHP_METHOD(DateTimeZone, __construct)
{
	char *tz;
	int tz_len;
	php_timezone_obj *tzobj;
	zend_error_handling error_handling;
	
	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
	if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tz, &tz_len)) {
		tzobj = zend_object_store_get_object(getThis() TSRMLS_CC);
		if (FAILURE == timezone_initialize(tzobj, tz TSRMLS_CC)) {
			ZVAL_NULL(getThis());
		}
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);