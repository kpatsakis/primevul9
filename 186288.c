static int php_session_initialize(void) /* {{{ */
{
	zend_string *val = NULL;

	PS(session_status) = php_session_active;

	if (!PS(mod)) {
		PS(session_status) = php_session_disabled;
		php_error_docref(NULL, E_WARNING, "No storage module chosen - failed to initialize session");
		return FAILURE;
	}

	/* Open session handler first */
	if (PS(mod)->s_open(&PS(mod_data), PS(save_path), PS(session_name)) == FAILURE
		/* || PS(mod_data) == NULL */ /* FIXME: open must set valid PS(mod_data) with success */
	) {
		php_session_abort();
		php_error_docref(NULL, E_WARNING, "Failed to initialize storage module: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		return FAILURE;
	}

	/* If there is no ID, use session module to create one */
	if (!PS(id) || !ZSTR_VAL(PS(id))[0]) {
		if (PS(id)) {
			zend_string_release(PS(id));
		}
		PS(id) = PS(mod)->s_create_sid(&PS(mod_data));
		if (!PS(id)) {
			php_session_abort();
			zend_throw_error(NULL, "Failed to create session ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
			return FAILURE;
		}
		if (PS(use_cookies)) {
			PS(send_cookie) = 1;
		}
	} else if (PS(use_strict_mode) && PS(mod)->s_validate_sid &&
		PS(mod)->s_validate_sid(&PS(mod_data), PS(id)) == FAILURE) {
		if (PS(id)) {
			zend_string_release(PS(id));
		}
		PS(id) = PS(mod)->s_create_sid(&PS(mod_data));
		if (!PS(id)) {
			PS(id) = php_session_create_id(NULL);
		}
		if (PS(use_cookies)) {
			PS(send_cookie) = 1;
		}
	}

	if (php_session_reset_id() == FAILURE) {
		php_session_abort();
		return FAILURE;
	}

	/* Read data */
	php_session_track_init();
	if (PS(mod)->s_read(&PS(mod_data), PS(id), &val, PS(gc_maxlifetime)) == FAILURE) {
		php_session_abort();
		/* FYI: Some broken save handlers return FAILURE for non-existent session ID, this is incorrect */
		php_error_docref(NULL, E_WARNING, "Failed to read session data: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		return FAILURE;
	}

	/* GC must be done after read */
	php_session_gc(0);

	if (PS(session_vars)) {
		zend_string_release(PS(session_vars));
		PS(session_vars) = NULL;
	}
	if (val) {
		if (PS(lazy_write)) {
			PS(session_vars) = zend_string_copy(val);
		}
		php_session_decode(val);
		zend_string_release(val);
	}
	return SUCCESS;
}