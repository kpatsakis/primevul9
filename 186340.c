static PHP_FUNCTION(session_name)
{
	zend_string *name = NULL;
	zend_string *ini_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		return;
	}

	if (name && PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Cannot change session name when session is active");
		RETURN_FALSE;
	}

	if (name && SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Cannot change session name when headers already sent");
		RETURN_FALSE;
	}

	RETVAL_STRING(PS(session_name));

	if (name) {
		ini_name = zend_string_init("session.name", sizeof("session.name") - 1, 0);
		zend_alter_ini_entry(ini_name, name, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(ini_name);
	}
}