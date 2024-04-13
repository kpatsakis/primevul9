static int php_session_abort(void) /* {{{ */
{
	if (PS(session_status) == php_session_active) {
		if (PS(mod_data) || PS(mod_user_implemented)) {
			PS(mod)->s_close(&PS(mod_data));
		}
		PS(session_status) = php_session_none;
		return SUCCESS;
	}
	return FAILURE;
}