static void php_session_save_current_state(int write) /* {{{ */
{
	int ret = FAILURE;

	if (write) {
		IF_SESSION_VARS() {
			if (PS(mod_data) || PS(mod_user_implemented)) {
				zend_string *val;

				val = php_session_encode();
				if (val) {
					if (PS(lazy_write) && PS(session_vars)
						&& PS(mod)->s_update_timestamp
						&& PS(mod)->s_update_timestamp != php_session_update_timestamp
						&& ZSTR_LEN(val) == ZSTR_LEN(PS(session_vars))
						&& !memcmp(ZSTR_VAL(val), ZSTR_VAL(PS(session_vars)), ZSTR_LEN(val))
					) {
						ret = PS(mod)->s_update_timestamp(&PS(mod_data), PS(id), val, PS(gc_maxlifetime));
					} else {
						ret = PS(mod)->s_write(&PS(mod_data), PS(id), val, PS(gc_maxlifetime));
					}
					zend_string_release(val);
				} else {
					ret = PS(mod)->s_write(&PS(mod_data), PS(id), ZSTR_EMPTY_ALLOC(), PS(gc_maxlifetime));
				}
			}

			if ((ret == FAILURE) && !EG(exception)) {
				if (!PS(mod_user_implemented)) {
					php_error_docref(NULL, E_WARNING, "Failed to write session data (%s). Please "
									 "verify that the current setting of session.save_path "
									 "is correct (%s)",
									 PS(mod)->s_name,
									 PS(save_path));
				} else {
					php_error_docref(NULL, E_WARNING, "Failed to write session data using user "
									 "defined save handler. (session.save_path: %s)", PS(save_path));
				}
			}
		}
	}

	if (PS(mod_data) || PS(mod_user_implemented)) {
		PS(mod)->s_close(&PS(mod_data));
	}
}