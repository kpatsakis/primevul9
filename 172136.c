PHP_FUNCTION(filter_input)
{
	long   fetch_from, filter = FILTER_DEFAULT;
	zval **filter_args = NULL, **tmp;
	zval  *input = NULL;
	char *var;
	int var_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls|lZ", &fetch_from, &var, &var_len, &filter, &filter_args) == FAILURE) {
		return;
	}

	if (!PHP_FILTER_ID_EXISTS(filter)) {
		RETURN_FALSE;
	}

	input = php_filter_get_storage(fetch_from TSRMLS_CC);

	if (!input || !HASH_OF(input) || zend_hash_find(HASH_OF(input), var, var_len + 1, (void **)&tmp) != SUCCESS) {
		long filter_flags = 0;
		zval **option, **opt, **def;
		if (filter_args) {
			if (Z_TYPE_PP(filter_args) == IS_LONG) {
				filter_flags = Z_LVAL_PP(filter_args);
			} else if (Z_TYPE_PP(filter_args) == IS_ARRAY && zend_hash_find(HASH_OF(*filter_args), "flags", sizeof("flags"), (void **)&option) == SUCCESS) {
				PHP_FILTER_GET_LONG_OPT(option, filter_flags);
			}
			if (Z_TYPE_PP(filter_args) == IS_ARRAY && 
				zend_hash_find(HASH_OF(*filter_args), "options", sizeof("options"), (void **)&opt) == SUCCESS &&
				Z_TYPE_PP(opt) == IS_ARRAY &&
				zend_hash_find(HASH_OF(*opt), "default", sizeof("default"), (void **)&def) == SUCCESS
			) {
				MAKE_COPY_ZVAL(def, return_value);
				return;
			}
		}

		/* The FILTER_NULL_ON_FAILURE flag inverts the usual return values of
		 * the function: normally when validation fails false is returned, and
		 * when the input value doesn't exist NULL is returned. With the flag
		 * set, NULL and false should be returned, respectively. Ergo, although
		 * the code below looks incorrect, it's actually right. */
		if (filter_flags & FILTER_NULL_ON_FAILURE) {
			RETURN_FALSE;
		} else {
			RETURN_NULL();
		}
	}

	MAKE_COPY_ZVAL(tmp, return_value);

	php_filter_call(&return_value, filter, filter_args, 1, FILTER_REQUIRE_SCALAR TSRMLS_CC);
}