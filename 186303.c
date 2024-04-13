static PHP_INI_MH(OnUpdateCookieLifetime) /* {{{ */
{
	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;
	if (atol(ZSTR_VAL(new_value)) < 0) {
		php_error_docref(NULL, E_WARNING, "CookieLifetime cannot be negative");
		return FAILURE;
	}
	return OnUpdateLongGEZero(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}