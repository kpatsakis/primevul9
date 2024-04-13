PHPAPI int php_session_flush(int write) /* {{{ */
{
	if (PS(session_status) == php_session_active) {
		php_session_save_current_state(write);
		PS(session_status) = php_session_none;
		return SUCCESS;
	}
	return FAILURE;
}