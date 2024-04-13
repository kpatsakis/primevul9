static PHP_FUNCTION(session_write_close)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PS(session_status) != php_session_active) {
		RETURN_FALSE;
	}
	php_session_flush(1);
	RETURN_TRUE;
}