static PHP_FUNCTION(session_unset)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PS(session_status) != php_session_active) {
		RETURN_FALSE;
	}

	IF_SESSION_VARS() {
		zval *sess_var = Z_REFVAL(PS(http_session_vars));
		SEPARATE_ARRAY(sess_var);

		/* Clean $_SESSION. */
		zend_hash_clean(Z_ARRVAL_P(sess_var));
	}
	RETURN_TRUE;
}