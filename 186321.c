static PHP_RSHUTDOWN_FUNCTION(session) /* {{{ */
{
	int i;

	if (PS(session_status) == php_session_active) {
		zend_try {
			php_session_flush(1);
		} zend_end_try();
	}
	php_rshutdown_session_globals();

	/* this should NOT be done in php_rshutdown_session_globals() */
	for (i = 0; i < PS_NUM_APIS; i++) {
		if (!Z_ISUNDEF(PS(mod_user_names).names[i])) {
			zval_ptr_dtor(&PS(mod_user_names).names[i]);
			ZVAL_UNDEF(&PS(mod_user_names).names[i]);
		}
	}

	return SUCCESS;
}