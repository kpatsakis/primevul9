PHPAPI void php_add_session_var(zend_string *name) /* {{{ */
{
	IF_SESSION_VARS() {
		zval *sess_var = Z_REFVAL(PS(http_session_vars));
		SEPARATE_ARRAY(sess_var);
		if (!zend_hash_exists(Z_ARRVAL_P(sess_var), name)) {
			zval empty_var;
			ZVAL_NULL(&empty_var);
			zend_hash_update(Z_ARRVAL_P(sess_var), name, &empty_var);
		}
	}
}