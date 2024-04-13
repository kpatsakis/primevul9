static inline void php_rshutdown_session_globals(void) /* {{{ */
{
	/* Do NOT destroy PS(mod_user_names) here! */
	if (!Z_ISUNDEF(PS(http_session_vars))) {
		zval_ptr_dtor(&PS(http_session_vars));
		ZVAL_UNDEF(&PS(http_session_vars));
	}
	if (PS(mod_data) || PS(mod_user_implemented)) {
		zend_try {
			PS(mod)->s_close(&PS(mod_data));
		} zend_end_try();
	}
	if (PS(id)) {
		zend_string_release(PS(id));
		PS(id) = NULL;
	}

	if (PS(session_vars)) {
		zend_string_release(PS(session_vars));
		PS(session_vars) = NULL;
	}

	/* User save handlers may end up directly here by misuse, bugs in user script, etc. */
	/* Set session status to prevent error while restoring save handler INI value. */
	PS(session_status) = php_session_none;
}