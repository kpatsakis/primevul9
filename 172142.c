static zval *php_filter_get_storage(long arg TSRMLS_DC)/* {{{ */

{
	zval *array_ptr = NULL;

	switch (arg) {
		case PARSE_GET:
			array_ptr = IF_G(get_array);
			break;
		case PARSE_POST:
			array_ptr = IF_G(post_array);
			break;
		case PARSE_COOKIE:
			array_ptr = IF_G(cookie_array);
			break;
		case PARSE_SERVER:
			if (PG(auto_globals_jit)) {
				zend_is_auto_global("_SERVER", sizeof("_SERVER")-1 TSRMLS_CC);
			}
			array_ptr = IF_G(server_array);
			break;
		case PARSE_ENV:
			if (PG(auto_globals_jit)) {
				zend_is_auto_global("_ENV", sizeof("_ENV")-1 TSRMLS_CC);
			}
			array_ptr = IF_G(env_array) ? IF_G(env_array) : PG(http_globals)[TRACK_VARS_ENV];
			break;
		case PARSE_SESSION:
			/* FIXME: Implement session source */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "INPUT_SESSION is not yet implemented");
			break;
		case PARSE_REQUEST:
			/* FIXME: Implement request source */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "INPUT_REQUEST is not yet implemented");
			break;
	}

	return array_ptr;
}