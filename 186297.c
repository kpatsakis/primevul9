PHPAPI zval* php_set_session_var(zend_string *name, zval *state_val, php_unserialize_data_t *var_hash) /* {{{ */
{
	IF_SESSION_VARS() {
		zval *sess_var = Z_REFVAL(PS(http_session_vars));
		SEPARATE_ARRAY(sess_var);
		return zend_hash_update(Z_ARRVAL_P(sess_var), name, state_val);
	}
	return NULL;
}