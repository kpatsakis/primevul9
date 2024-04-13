static PHP_FUNCTION(session_create_id)
{
	zend_string *prefix = NULL, *new_id;
	smart_str id = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &prefix) == FAILURE) {
		return;
	}

	if (prefix && ZSTR_LEN(prefix)) {
		if (php_session_valid_key(ZSTR_VAL(prefix)) == FAILURE) {
			/* E_ERROR raised for security reason. */
			php_error_docref(NULL, E_WARNING, "Prefix cannot contain special characters. Only aphanumeric, ',', '-' are allowed");
			RETURN_FALSE;
		} else {
			smart_str_append(&id, prefix);
		}
	}

	if (!PS(in_save_handler) && PS(session_status) == php_session_active) {
		int limit = 3;
		while (limit--) {
			new_id = PS(mod)->s_create_sid(&PS(mod_data));
			if (!PS(mod)->s_validate_sid) {
				break;
			} else {
				/* Detect collision and retry */
				if (PS(mod)->s_validate_sid(&PS(mod_data), new_id) == FAILURE) {
					zend_string_release(new_id);
					new_id = NULL;
					continue;
				}
				break;
			}
		}
	} else {
		new_id = php_session_create_id(NULL);
	}

	if (new_id) {
		smart_str_append(&id, new_id);
		zend_string_release(new_id);
	} else {
		smart_str_free(&id);
		php_error_docref(NULL, E_WARNING, "Failed to create new ID");
		RETURN_FALSE;
	}
	smart_str_0(&id);
	RETVAL_NEW_STR(id.s);
}