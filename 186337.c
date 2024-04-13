static PHP_FUNCTION(session_regenerate_id)
{
	zend_bool del_ses = 0;
	zend_string *data;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &del_ses) == FAILURE) {
		return;
	}

	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL, E_WARNING, "Cannot regenerate session id - session is not active");
		RETURN_FALSE;
	}

	if (SG(headers_sent)) {
		php_error_docref(NULL, E_WARNING, "Cannot regenerate session id - headers already sent");
		RETURN_FALSE;
	}

	/* Process old session data */
	if (del_ses) {
		if (PS(mod)->s_destroy(&PS(mod_data), PS(id)) == FAILURE) {
			PS(mod)->s_close(&PS(mod_data));
			PS(session_status) = php_session_none;
			php_error_docref(NULL, E_WARNING, "Session object destruction failed.  ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
			RETURN_FALSE;
		}
	} else {
		int ret;
		data = php_session_encode();
		if (data) {
			ret = PS(mod)->s_write(&PS(mod_data), PS(id), data, PS(gc_maxlifetime));
			zend_string_release(data);
		} else {
			ret = PS(mod)->s_write(&PS(mod_data), PS(id), ZSTR_EMPTY_ALLOC(), PS(gc_maxlifetime));
		}
		if (ret == FAILURE) {
			PS(mod)->s_close(&PS(mod_data));
			PS(session_status) = php_session_none;
			php_error_docref(NULL, E_WARNING, "Session write failed. ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
			RETURN_FALSE;
		}
	}
	PS(mod)->s_close(&PS(mod_data));

	/* New session data */
	if (PS(session_vars)) {
		zend_string_release(PS(session_vars));
		PS(session_vars) = NULL;
	}
	zend_string_release(PS(id));
	PS(id) = NULL;

	if (PS(mod)->s_open(&PS(mod_data), PS(save_path), PS(session_name)) == FAILURE) {
		PS(session_status) = php_session_none;
		zend_throw_error(NULL, "Failed to open session: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		RETURN_FALSE;
	}

	PS(id) = PS(mod)->s_create_sid(&PS(mod_data));
	if (!PS(id)) {
		PS(session_status) = php_session_none;
		zend_throw_error(NULL, "Failed to create new session ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		RETURN_FALSE;
	}
	if (PS(use_strict_mode) && PS(mod)->s_validate_sid &&
		PS(mod)->s_validate_sid(&PS(mod_data), PS(id)) == FAILURE) {
		zend_string_release(PS(id));
		PS(id) = PS(mod)->s_create_sid(&PS(mod_data));
		if (!PS(id)) {
			PS(mod)->s_close(&PS(mod_data));
			PS(session_status) = php_session_none;
			zend_throw_error(NULL, "Failed to create session ID by collision: %s (path: %s)", PS(mod)->s_name, PS(save_path));
			RETURN_FALSE;
		}
	}
	/* Read is required to make new session data at this point. */
	if (PS(mod)->s_read(&PS(mod_data), PS(id), &data, PS(gc_maxlifetime)) == FAILURE) {
		PS(mod)->s_close(&PS(mod_data));
		PS(session_status) = php_session_none;
		zend_throw_error(NULL, "Failed to create(read) session ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		RETURN_FALSE;
	}
	if (data) {
		zend_string_release(data);
	}

	if (PS(use_cookies)) {
		PS(send_cookie) = 1;
	}
	if (php_session_reset_id() == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}