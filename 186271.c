static PHP_INI_MH(OnUpdateSidLength) /* {{{ */
{
	zend_long val;
	char *endptr = NULL;

	SESSION_CHECK_OUTPUT_STATE;
	SESSION_CHECK_ACTIVE_STATE;
	val = ZEND_STRTOL(ZSTR_VAL(new_value), &endptr, 10);
	if (endptr && (*endptr == '\0')
		&& val >= 22 && val <= PS_MAX_SID_LENGTH) {
		/* Numeric value */
		PS(sid_length) = val;
		return SUCCESS;
	}

	php_error_docref(NULL, E_WARNING, "session.configuration 'session.sid_length' must be between 22 and 256.");
	return FAILURE;
}