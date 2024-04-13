PS_SERIALIZER_DECODE_FUNC(php_binary) /* {{{ */
{
	const char *p;
	const char *endptr = val + vallen;
	int namelen;
	zend_string *name;
	php_unserialize_data_t var_hash;
	zval *current, rv;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	for (p = val; p < endptr; ) {
		namelen = ((unsigned char)(*p)) & (~PS_BIN_UNDEF);

		if (namelen < 0 || namelen > PS_BIN_MAX || (p + namelen) >= endptr) {
			PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
			return FAILURE;
		}

		name = zend_string_init(p + 1, namelen, 0);
		p += namelen + 1;
		current = var_tmp_var(&var_hash);

		if (php_var_unserialize(current, (const unsigned char **) &p, (const unsigned char *) endptr, &var_hash)) {
			ZVAL_PTR(&rv, current);
			php_set_session_var(name, &rv, &var_hash);
		} else {
			zend_string_release(name);
			php_session_normalize_vars();
			PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
			return FAILURE;
		}
		zend_string_release(name);
	}

	php_session_normalize_vars();
	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);

	return SUCCESS;
}