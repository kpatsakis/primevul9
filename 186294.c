static PHP_INI_MH(OnUpdateSessionString) /* {{{ */
{
	SESSION_CHECK_ACTIVE_STATE;
	SESSION_CHECK_OUTPUT_STATE;
	return OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}