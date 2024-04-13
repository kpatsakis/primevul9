static PHP_FUNCTION(session_cache_expire)
{
	zval *expires = NULL;
	zend_string *ini_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|z", &expires) == FAILURE) {
		return;
	}

	if (expires && PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Cannot change cache expire when session is active");
		RETURN_LONG(PS(cache_expire));
	}

	if (expires && SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Cannot change cache expire when headers already sent");
		RETURN_FALSE;
	}

	RETVAL_LONG(PS(cache_expire));

	if (expires) {
		convert_to_string_ex(expires);
		ini_name = zend_string_init("session.cache_expire", sizeof("session.cache_expire") - 1, 0);
		zend_alter_ini_entry(ini_name, Z_STR_P(expires), ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
		zend_string_release(ini_name);
	}
}