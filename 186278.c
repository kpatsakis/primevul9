static int php_rinit_session(zend_bool auto_start) /* {{{ */
{
	php_rinit_session_globals();

	if (PS(mod) == NULL) {
		char *value;

		value = zend_ini_string("session.save_handler", sizeof("session.save_handler") - 1, 0);
		if (value) {
			PS(mod) = _php_find_ps_module(value);
		}
	}

	if (PS(serializer) == NULL) {
		char *value;

		value = zend_ini_string("session.serialize_handler", sizeof("session.serialize_handler") - 1, 0);
		if (value) {
			PS(serializer) = _php_find_ps_serializer(value);
		}
	}

	if (PS(mod) == NULL || PS(serializer) == NULL) {
		/* current status is unusable */
		PS(session_status) = php_session_disabled;
		return SUCCESS;
	}

	if (auto_start) {
		php_session_start();
	}

	return SUCCESS;
} /* }}} */