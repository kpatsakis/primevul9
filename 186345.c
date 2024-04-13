PS_SERIALIZER_DECODE_FUNC(php) /* {{{ */
{
	const char *p, *q;
	const char *endptr = val + vallen;
	ptrdiff_t namelen;
	zend_string *name;
	int retval = SUCCESS;
	php_unserialize_data_t var_hash;
	zval *current, rv;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	p = val;

	while (p < endptr) {
		q = p;
		while (*q != PS_DELIMITER) {
			if (++q >= endptr) goto break_outer_loop;
		}

		namelen = q - p;
		name = zend_string_init(p, namelen, 0);
		q++;

		current = var_tmp_var(&var_hash);
		if (php_var_unserialize(current, (const unsigned char **)&q, (const unsigned char *)endptr, &var_hash)) {
			ZVAL_PTR(&rv, current);
			php_set_session_var(name, &rv, &var_hash);
		} else {
			zend_string_release(name);
			retval = FAILURE;
			goto break_outer_loop;
		}
		zend_string_release(name);
		p = q;
	}

break_outer_loop:
	php_session_normalize_vars();

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);

	return retval;
}