static PHP_FUNCTION(session_gc)
{
	zend_long num;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL, E_WARNING, "Session is not active");
		RETURN_FALSE;
	}

	num = php_session_gc(1);
	if (num < 0) {
		RETURN_FALSE;
	}

	RETURN_LONG(num);
}