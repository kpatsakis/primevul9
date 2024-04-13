static PHP_FUNCTION(session_module_name)
{
	zend_string *name = NULL;
	zend_string *ini_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		return;
	}

	if (name && PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Cannot change save handler module when session is active");
		RETURN_FALSE;
	}

	if (name && SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Cannot change save handler module when headers already sent");
		RETURN_FALSE;
	}

	/* Set return_value to current module name */
	if (PS(mod) && PS(mod)->s_name) {
		RETVAL_STRING(PS(mod)->s_name);
	} else {
		RETVAL_EMPTY_STRING();
	}

	if (name) {
		if (!_php_find_ps_module(ZSTR_VAL(name))) {
			php_error_docref(NULL, E_WARNING, "Cannot find named PHP session module (%s)", ZSTR_VAL(name));

			zval_dtor(return_value);
			RETURN_FALSE;
		}
		if (PS(mod_data) || PS(mod_user_implemented)) {
			PS(mod)->s_close(&PS(mod_data));
		}
		PS(mod_data) = NULL;

		ini_name = zend_string_init("session.save_handler", sizeof("session.save_handler") - 1, 0);
		zend_alter_ini_entry(ini_name, name, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
		zend_string_release(ini_name);
	}
}