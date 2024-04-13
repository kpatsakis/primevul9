static PHP_FUNCTION(session_decode)
{
	zend_string *str = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &str) == FAILURE) {
		return;
	}

	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL, E_WARNING, "Session is not active. You cannot decode session data");
		RETURN_FALSE;
	}

	if (php_session_decode(str) == FAILURE) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}