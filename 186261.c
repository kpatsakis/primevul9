PHPAPI zval* php_get_session_var(zend_string *name) /* {{{ */
{
	IF_SESSION_VARS() {
		return zend_hash_find(Z_ARRVAL_P(Z_REFVAL(PS(http_session_vars))), name);
	}
	return NULL;
}