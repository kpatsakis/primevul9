static void php_session_rfc1867_cleanup(php_session_rfc1867_progress *progress) /* {{{ */
{
	php_session_initialize();
	PS(session_status) = php_session_active;
	IF_SESSION_VARS() {
		zval *sess_var = Z_REFVAL(PS(http_session_vars));
		SEPARATE_ARRAY(sess_var);
		zend_hash_del(Z_ARRVAL_P(sess_var), progress->key.s);
	}
	php_session_flush(1);
} /* }}} */