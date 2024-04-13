static PHP_FUNCTION(session_reset)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PS(session_status) != php_session_active) {
		RETURN_FALSE;
	}
	php_session_reset();
	RETURN_TRUE;
}