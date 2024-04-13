static int php_session_reset(void) /* {{{ */
{
	if (PS(session_status) == php_session_active
		&& php_session_initialize() == SUCCESS) {
		return SUCCESS;
	}
	return FAILURE;
}