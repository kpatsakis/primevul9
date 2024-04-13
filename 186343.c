static PHP_INI_MH(OnUpdateSessionBool) /* {{{ */
{
	SESSION_CHECK_OUTPUT_STATE;
	SESSION_CHECK_ACTIVE_STATE;
	return OnUpdateBool(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
}