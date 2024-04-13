static PHP_FUNCTION(session_abort)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PS(session_status) != php_session_active) {
		RETURN_FALSE;
	}
	php_session_abort();
	RETURN_TRUE;
}