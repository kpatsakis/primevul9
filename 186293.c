static PHP_FUNCTION(session_destroy)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(php_session_destroy() == SUCCESS);
}