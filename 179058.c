*/
PHP_FUNCTION(timezone_open)
{
	char *tz;
	int   tz_len;
	php_timezone_obj *tzobj;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &tz, &tz_len) == FAILURE) {
		RETURN_FALSE;
	}
	tzobj = zend_object_store_get_object(php_date_instantiate(date_ce_timezone, return_value TSRMLS_CC) TSRMLS_CC);
	if (SUCCESS != timezone_initialize(tzobj, tz TSRMLS_CC)) {
		RETURN_FALSE;
	}