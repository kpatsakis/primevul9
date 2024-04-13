static PHP_INI_MH(OnUpdateTransSid) /* {{{ */
{
	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;

	if (!strncasecmp(ZSTR_VAL(new_value), "on", sizeof("on"))) {
		PS(use_trans_sid) = (zend_bool) 1;
	} else {
		PS(use_trans_sid) = (zend_bool) atoi(ZSTR_VAL(new_value));
	}

	return SUCCESS;
}