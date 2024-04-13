static PHP_FUNCTION(session_start)
{
	zval *options = NULL;
	zval *value;
	zend_ulong num_idx;
	zend_string *str_idx;
	zend_long read_and_close = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a", &options) == FAILURE) {
		RETURN_FALSE;
	}

	if (PS(session_status) == php_session_active) {
		php_error_docref(NULL, E_NOTICE, "A session had already been started - ignoring");
		RETURN_TRUE;
	}

	/*
	 * TODO: To prevent unusable session with trans sid, actual output started status is
	 * required. i.e. There shouldn't be any outputs in output buffer, otherwise session
	 * module is unable to rewrite output.
	 */
	if (PS(use_cookies) && SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Cannot start session when headers already sent");
		RETURN_FALSE;
	}

	/* set options */
	if (options) {
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(options), num_idx, str_idx, value) {
			if (str_idx) {
				switch(Z_TYPE_P(value)) {
					case IS_STRING:
					case IS_TRUE:
					case IS_FALSE:
					case IS_LONG:
						if (zend_string_equals_literal(str_idx, "read_and_close")) {
							read_and_close = zval_get_long(value);
						} else {
							zend_string *val = zval_get_string(value);
							if (php_session_start_set_ini(str_idx, val) == FAILURE) {
								php_error_docref(NULL, E_WARNING, "Setting option '%s' failed", ZSTR_VAL(str_idx));
							}
							zend_string_release(val);
						}
						break;
					default:
						php_error_docref(NULL, E_WARNING, "Option(%s) value must be string, boolean or long", ZSTR_VAL(str_idx));
						break;
				}
			}
			(void) num_idx;
		} ZEND_HASH_FOREACH_END();
	}

	php_session_start();

	if (PS(session_status) != php_session_active) {
		IF_SESSION_VARS() {
			zval *sess_var = Z_REFVAL(PS(http_session_vars));
			SEPARATE_ARRAY(sess_var);
			/* Clean $_SESSION. */
			zend_hash_clean(Z_ARRVAL_P(sess_var));
		}
		RETURN_FALSE;
	}

	if (read_and_close) {
		php_session_flush(0);
	}

	RETURN_TRUE;
}