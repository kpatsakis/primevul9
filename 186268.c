PHPAPI int php_session_destroy(void) /* {{{ */
{
	int retval = SUCCESS;

	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL, E_WARNING, "Trying to destroy uninitialized session");
		return FAILURE;
	}

	if (PS(id) && PS(mod)->s_destroy(&PS(mod_data), PS(id)) == FAILURE) {
		retval = FAILURE;
		php_error_docref(NULL, E_WARNING, "Session object destruction failed");
	}

	php_rshutdown_session_globals();
	php_rinit_session_globals();

	return retval;
}