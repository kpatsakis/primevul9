static void php_session_track_init(void) /* {{{ */
{
	zval session_vars;
	zend_string *var_name = zend_string_init("_SESSION", sizeof("_SESSION") - 1, 0);
	/* Unconditionally destroy existing array -- possible dirty data */
	zend_delete_global_variable(var_name);

	if (!Z_ISUNDEF(PS(http_session_vars))) {
		zval_ptr_dtor(&PS(http_session_vars));
	}

	array_init(&session_vars);
	ZVAL_NEW_REF(&PS(http_session_vars), &session_vars);
	Z_ADDREF_P(&PS(http_session_vars));
	zend_hash_update_ind(&EG(symbol_table), var_name, &PS(http_session_vars));
	zend_string_release(var_name);
}