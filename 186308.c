static PHP_FUNCTION(session_id)
{
	zend_string *name = NULL;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc, "|S", &name) == FAILURE) {
		return;
	}

	if (name && PS(use_cookies) && SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Cannot change session id when headers already sent");
		RETURN_FALSE;
	}

	if (PS(id)) {
		/* keep compatibility for "\0" characters ???
		 * see: ext/session/tests/session_id_error3.phpt */
		size_t len = strlen(ZSTR_VAL(PS(id)));
		if (UNEXPECTED(len != ZSTR_LEN(PS(id)))) {
			RETVAL_NEW_STR(zend_string_init(ZSTR_VAL(PS(id)), len, 0));
		} else {
			RETVAL_STR_COPY(PS(id));
		}
	} else {
		RETVAL_EMPTY_STRING();
	}

	if (name) {
		if (PS(id)) {
			zend_string_release(PS(id));
		}
		PS(id) = zend_string_copy(name);
	}
}