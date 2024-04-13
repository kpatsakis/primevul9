static PHP_INI_MH(OnUpdateSidBits) /* {{{ */
{
	zend_long val;
	char *endptr = NULL;

	SESSION_CHECK_OUTPUT_STATE;
	SESSION_CHECK_ACTIVE_STATE;
	val = ZEND_STRTOL(ZSTR_VAL(new_value), &endptr, 10);
	if (endptr && (*endptr == '\0')
		&& val >= 4 && val <=6) {
		/* Numeric value */
		PS(sid_bits_per_character) = val;
		return SUCCESS;
	}

	php_error_docref(NULL, E_WARNING, "session.configuration 'session.sid_bits_per_character' must be between 4 and 6.");
	return FAILURE;
}