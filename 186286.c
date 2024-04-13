static int php_session_send_cookie(void) /* {{{ */
{
	smart_str ncookie = {0};
	zend_string *date_fmt = NULL;
	zend_string *e_session_name, *e_id;

	if (SG(headers_sent)) {
		const char *output_start_filename = php_output_get_start_filename();
		int output_start_lineno = php_output_get_start_lineno();

		if (output_start_filename) {
			php_error_docref(NULL, E_WARNING, "Cannot send session cookie - headers already sent by (output started at %s:%d)", output_start_filename, output_start_lineno);
		} else {
			php_error_docref(NULL, E_WARNING, "Cannot send session cookie - headers already sent");
		}
		return FAILURE;
	}

	/* URL encode session_name and id because they might be user supplied */
	e_session_name = php_url_encode(PS(session_name), strlen(PS(session_name)));
	e_id = php_url_encode(ZSTR_VAL(PS(id)), ZSTR_LEN(PS(id)));

	smart_str_appendl(&ncookie, "Set-Cookie: ", sizeof("Set-Cookie: ")-1);
	smart_str_appendl(&ncookie, ZSTR_VAL(e_session_name), ZSTR_LEN(e_session_name));
	smart_str_appendc(&ncookie, '=');
	smart_str_appendl(&ncookie, ZSTR_VAL(e_id), ZSTR_LEN(e_id));

	zend_string_release(e_session_name);
	zend_string_release(e_id);

	if (PS(cookie_lifetime) > 0) {
		struct timeval tv;
		time_t t;

		gettimeofday(&tv, NULL);
		t = tv.tv_sec + PS(cookie_lifetime);

		if (t > 0) {
			date_fmt = php_format_date("D, d-M-Y H:i:s T", sizeof("D, d-M-Y H:i:s T")-1, t, 0);
			smart_str_appends(&ncookie, COOKIE_EXPIRES);
			smart_str_appendl(&ncookie, ZSTR_VAL(date_fmt), ZSTR_LEN(date_fmt));
			zend_string_release(date_fmt);

			smart_str_appends(&ncookie, COOKIE_MAX_AGE);
			smart_str_append_long(&ncookie, PS(cookie_lifetime));
		}
	}

	if (PS(cookie_path)[0]) {
		smart_str_appends(&ncookie, COOKIE_PATH);
		smart_str_appends(&ncookie, PS(cookie_path));
	}

	if (PS(cookie_domain)[0]) {
		smart_str_appends(&ncookie, COOKIE_DOMAIN);
		smart_str_appends(&ncookie, PS(cookie_domain));
	}

	if (PS(cookie_secure)) {
		smart_str_appends(&ncookie, COOKIE_SECURE);
	}

	if (PS(cookie_httponly)) {
		smart_str_appends(&ncookie, COOKIE_HTTPONLY);
	}

	smart_str_0(&ncookie);

	php_session_remove_cookie(); /* remove already sent session ID cookie */
	/*	'replace' must be 0 here, else a previous Set-Cookie
		header, probably sent with setcookie() will be replaced! */
	sapi_add_header_ex(estrndup(ZSTR_VAL(ncookie.s), ZSTR_LEN(ncookie.s)), ZSTR_LEN(ncookie.s), 0, 0);
	smart_str_free(&ncookie);

	return SUCCESS;
}