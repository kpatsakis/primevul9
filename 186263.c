static PHP_FUNCTION(session_set_cookie_params)
{
	zval *lifetime;
	zend_string *path = NULL, *domain = NULL;
	int argc = ZEND_NUM_ARGS();
	zend_bool secure = 0, httponly = 0;
	zend_string *ini_name;

	if (!PS(use_cookies) ||
		zend_parse_parameters(argc, "z|SSbb", &lifetime, &path, &domain, &secure, &httponly) == FAILURE) {
		return;
	}


	if (PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_WARNING, "Cannot change session cookie parameters when session is active");
		RETURN_FALSE;
	}

	if (SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Cannot change session cookie parameters when headers already sent");
		RETURN_FALSE;
	}

	convert_to_string_ex(lifetime);

	ini_name = zend_string_init("session.cookie_lifetime", sizeof("session.cookie_lifetime") - 1, 0);
	if (zend_alter_ini_entry(ini_name,  Z_STR_P(lifetime), PHP_INI_USER, PHP_INI_STAGE_RUNTIME) == FAILURE) {
		zend_string_release(ini_name);
		RETURN_FALSE;
	}
	zend_string_release(ini_name);

	if (path) {
		ini_name = zend_string_init("session.cookie_path", sizeof("session.cookie_path") - 1, 0);
		if (zend_alter_ini_entry(ini_name, path, PHP_INI_USER, PHP_INI_STAGE_RUNTIME) == FAILURE) {
			zend_string_release(ini_name);
			RETURN_FALSE;
		}
		zend_string_release(ini_name);
	}
	if (domain) {
		ini_name = zend_string_init("session.cookie_domain", sizeof("session.cookie_domain") - 1, 0);
		if (zend_alter_ini_entry(ini_name, domain, PHP_INI_USER, PHP_INI_STAGE_RUNTIME) == FAILURE) {
			zend_string_release(ini_name);
			RETURN_FALSE;
		}
		zend_string_release(ini_name);
	}

	if (argc > 3) {
		ini_name = zend_string_init("session.cookie_secure", sizeof("session.cookie_secure") - 1, 0);
		if (zend_alter_ini_entry_chars(ini_name, secure ? "1" : "0", 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME) == FAILURE) {
			zend_string_release(ini_name);
			RETURN_FALSE;
		}
		zend_string_release(ini_name);
	}
	if (argc > 4) {
		ini_name = zend_string_init("session.cookie_httponly", sizeof("session.cookie_httponly") - 1, 0);
		if (zend_alter_ini_entry_chars(ini_name, httponly ? "1" : "0", 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME) == FAILURE) {
			zend_string_release(ini_name);
			RETURN_FALSE;
		}
		zend_string_release(ini_name);
	}

	RETURN_TRUE;
}