static PHP_FUNCTION(session_save_path)
{
	zend_string *name = NULL;
	zend_string *ini_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		return;
	}

	if (name && PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Cannot change save path when session is active");
		RETURN_FALSE;
	}

	if (name && SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Cannot change save path when headers already sent");
		RETURN_FALSE;
	}

	RETVAL_STRING(PS(save_path));

	if (name) {
		if (memchr(ZSTR_VAL(name), '\0', ZSTR_LEN(name)) != NULL) {
			php_error_docref(NULL, E_WARNING, "The save_path cannot contain NULL characters");
			zval_dtor(return_value);
			RETURN_FALSE;
		}
		ini_name = zend_string_init("session.save_path", sizeof("session.save_path") - 1, 0);
		zend_alter_ini_entry(ini_name, name, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(ini_name);
	}
}