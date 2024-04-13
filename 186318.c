PS_SERIALIZER_DECODE_FUNC(php_serialize) /* {{{ */
{
	const char *endptr = val + vallen;
	zval session_vars;
	php_unserialize_data_t var_hash;
	int result;
	zend_string *var_name = zend_string_init("_SESSION", sizeof("_SESSION") - 1, 0);

	ZVAL_NULL(&session_vars);
	PHP_VAR_UNSERIALIZE_INIT(var_hash);
	result = php_var_unserialize(
		&session_vars, (const unsigned char **)&val, (const unsigned char *)endptr, &var_hash);
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
	if (!result) {
		zval_ptr_dtor(&session_vars);
		ZVAL_NULL(&session_vars);
	}

	if (!Z_ISUNDEF(PS(http_session_vars))) {
		zval_ptr_dtor(&PS(http_session_vars));
	}
	if (Z_TYPE(session_vars) == IS_NULL) {
		array_init(&session_vars);
	}
	ZVAL_NEW_REF(&PS(http_session_vars), &session_vars);
	Z_ADDREF_P(&PS(http_session_vars));
	zend_hash_update_ind(&EG(symbol_table), var_name, &PS(http_session_vars));
	zend_string_release(var_name);
	return SUCCESS;
}