static PHP_INI_MH(OnUpdateName) /* {{{ */
{
	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;

	/* Numeric session.name won't work at all */
	if ((!ZSTR_LEN(new_value) || is_numeric_string(ZSTR_VAL(new_value), ZSTR_LEN(new_value), NULL, NULL, 0))) {
		int err_type;

		if (stage == ZEND_INI_STAGE_RUNTIME || stage == ZEND_INI_STAGE_ACTIVATE || stage == ZEND_INI_STAGE_STARTUP) {
			err_type = E_WARNING;
		} else {
			err_type = E_ERROR;
		}

		/* Do not output error when restoring ini options. */
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			php_error_docref(NULL, err_type, "session.name cannot be a numeric or empty '%s'", ZSTR_VAL(new_value));
		}
		return FAILURE;
	}

	return OnUpdateStringUnempty(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}