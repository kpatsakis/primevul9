static unsigned int php_sapi_filter(int arg, char *var, char **val, unsigned int val_len, unsigned int *new_val_len TSRMLS_DC) /* {{{ */
{
	zval  new_var, raw_var;
	zval *array_ptr = NULL, *orig_array_ptr = NULL;
	int retval = 0;

	assert(*val != NULL);

#define PARSE_CASE(s,a,t)                    \
		case s:                              \
			if (!IF_G(a)) {                  \
				ALLOC_ZVAL(array_ptr);       \
				array_init(array_ptr);       \
				INIT_PZVAL(array_ptr);       \
				IF_G(a) = array_ptr;         \
			} else {                         \
				array_ptr = IF_G(a);         \
			}                                \
			orig_array_ptr = PG(http_globals)[t]; \
			break;

	switch (arg) {
		PARSE_CASE(PARSE_POST,    post_array,    TRACK_VARS_POST)
		PARSE_CASE(PARSE_GET,     get_array,     TRACK_VARS_GET)
		PARSE_CASE(PARSE_COOKIE,  cookie_array,  TRACK_VARS_COOKIE)
		PARSE_CASE(PARSE_SERVER,  server_array,  TRACK_VARS_SERVER)
		PARSE_CASE(PARSE_ENV,     env_array,     TRACK_VARS_ENV)

		case PARSE_STRING: /* PARSE_STRING is used by parse_str() function */
			retval = 1;
			break;
	}

	/* 
	 * According to rfc2965, more specific paths are listed above the less specific ones.
	 * If we encounter a duplicate cookie name, we should skip it, since it is not possible
	 * to have the same (plain text) cookie name for the same path and we should not overwrite
	 * more specific cookies with the less specific ones.
	*/
	if (arg == PARSE_COOKIE && orig_array_ptr && zend_symtable_exists(Z_ARRVAL_P(orig_array_ptr), var, strlen(var)+1)) {
		return 0;
	}

	if (array_ptr) {
		/* Store the RAW variable internally */
		Z_STRLEN(raw_var) = val_len;
		Z_STRVAL(raw_var) = estrndup(*val, val_len);
		Z_TYPE(raw_var) = IS_STRING;

		php_register_variable_ex(var, &raw_var, array_ptr TSRMLS_CC);
	}

	if (val_len) {
		/* Register mangled variable */
		Z_STRLEN(new_var) = val_len;
		Z_TYPE(new_var) = IS_STRING;

		if (IF_G(default_filter) != FILTER_UNSAFE_RAW) {
			zval *tmp_new_var = &new_var;
			Z_STRVAL(new_var) = estrndup(*val, val_len);
			INIT_PZVAL(tmp_new_var);
			php_zval_filter(&tmp_new_var, IF_G(default_filter), IF_G(default_filter_flags), NULL, NULL/*charset*/, 0 TSRMLS_CC);
		} else {
			Z_STRVAL(new_var) = estrndup(*val, val_len);
		}
	} else { /* empty string */
		ZVAL_EMPTY_STRING(&new_var);
	}

	if (orig_array_ptr) {
		php_register_variable_ex(var, &new_var, orig_array_ptr TSRMLS_CC);
	}

	if (retval) {
		if (new_val_len) {
			*new_val_len = Z_STRLEN(new_var);
		}
		efree(*val);
		if (Z_STRLEN(new_var)) {
			*val = estrndup(Z_STRVAL(new_var), Z_STRLEN(new_var));
		} else {
			*val = estrdup("");
		}
		zval_dtor(&new_var);
	}

	return retval;
}