static PHP_FUNCTION(session_cache_limiter)
{
	zend_string *limiter = NULL;
	zend_string *ini_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &limiter) == FAILURE) {
		return;
	}

	if (limiter && PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Cannot change cache limiter when session is active");
		RETURN_FALSE;
	}

	if (limiter && SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Cannot change cache limiter when headers already sent");
		RETURN_FALSE;
	}

	RETVAL_STRING(PS(cache_limiter));

	if (limiter) {
		ini_name = zend_string_init("session.cache_limiter", sizeof("session.cache_limiter") - 1, 0);
		zend_alter_ini_entry(ini_name, limiter, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(ini_name);
	}
}