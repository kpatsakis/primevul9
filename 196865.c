static void php_session_initialize(TSRMLS_D) /* {{{ */
{
	char *val = NULL;
	int vallen;

	if (!PS(mod)) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "No storage module chosen - failed to initialize session");
		return;
	}

	/* Open session handler first */
	if (PS(mod)->s_open(&PS(mod_data), PS(save_path), PS(session_name) TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Failed to initialize storage module: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		return;
	}

	/* If there is no ID, use session module to create one */
	if (!PS(id)) {
		PS(id) = PS(mod)->s_create_sid(&PS(mod_data), NULL TSRMLS_CC);
		if (!PS(id)) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Failed to create session ID: %s (path: %s)", PS(mod)->s_name, PS(save_path));
			return;
		}
		if (PS(use_cookies)) {
			PS(send_cookie) = 1;
		}
	}

	/* Set session ID for compatibility for older/3rd party save handlers */
	if (!PS(use_strict_mode)) {
		php_session_reset_id(TSRMLS_C);
		PS(session_status) = php_session_active;
	}

	/* Read data */
	php_session_track_init(TSRMLS_C);
	if (PS(mod)->s_read(&PS(mod_data), PS(id), &val, &vallen TSRMLS_CC) == FAILURE) {
		/* Some broken save handler implementation returns FAILURE for non-existent session ID */
		/* It's better to raise error for this, but disabled error for better compatibility */
		/*
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Failed to read session data: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		*/
	}
	/* Set session ID if session read didn't activated session */
	if (PS(use_strict_mode) && PS(session_status) != php_session_active) {
		php_session_reset_id(TSRMLS_C);
		PS(session_status) = php_session_active;
	}
	if (val) {
		php_session_decode(val, vallen TSRMLS_CC);
		efree(val);
	}

	if (!PS(use_cookies) && PS(send_cookie)) {
		if (PS(use_trans_sid) && !PS(use_only_cookies)) {
			PS(apply_trans_sid) = 1;
		}
		PS(send_cookie) = 0;
	}
}