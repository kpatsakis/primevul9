PHPAPI int php_session_start(void) /* {{{ */
{
	zval *ppid;
	zval *data;
	char *p, *value;
	size_t lensess;

	switch (PS(session_status)) {
		case php_session_active:
			php_error(E_NOTICE, "A session had already been started - ignoring session_start()");
			return FAILURE;
			break;

		case php_session_disabled:
			value = zend_ini_string("session.save_handler", sizeof("session.save_handler") - 1, 0);
			if (!PS(mod) && value) {
				PS(mod) = _php_find_ps_module(value);
				if (!PS(mod)) {
					php_error_docref(NULL, E_WARNING, "Cannot find save handler '%s' - session startup failed", value);
					return FAILURE;
				}
			}
			value = zend_ini_string("session.serialize_handler", sizeof("session.serialize_handler") - 1, 0);
			if (!PS(serializer) && value) {
				PS(serializer) = _php_find_ps_serializer(value);
				if (!PS(serializer)) {
					php_error_docref(NULL, E_WARNING, "Cannot find serialization handler '%s' - session startup failed", value);
					return FAILURE;
				}
			}
			PS(session_status) = php_session_none;
			/* Fall through */

		case php_session_none:
		default:
			/* Setup internal flags */
			PS(define_sid) = !PS(use_only_cookies); /* SID constant is defined when non-cookie ID is used */
			PS(send_cookie) = PS(use_cookies) || PS(use_only_cookies);
	}

	lensess = strlen(PS(session_name));

	/*
	 * Cookies are preferred, because initially cookie and get
	 * variables will be available.
	 * URL/POST session ID may be used when use_only_cookies=Off.
	 * session.use_strice_mode=On prevents session adoption.
	 * Session based file upload progress uses non-cookie ID.
	 */

	if (!PS(id)) {
		if (PS(use_cookies) && (data = zend_hash_str_find(&EG(symbol_table), "_COOKIE", sizeof("_COOKIE") - 1))) {
			ZVAL_DEREF(data);
			if (Z_TYPE_P(data) == IS_ARRAY && (ppid = zend_hash_str_find(Z_ARRVAL_P(data), PS(session_name), lensess))) {
				ppid2sid(ppid);
				PS(send_cookie) = 0;
				PS(define_sid) = 0;
			}
		}
		/* Initilize session ID from non cookie values */
		if (!PS(use_only_cookies)) {
			if (!PS(id) && (data = zend_hash_str_find(&EG(symbol_table), "_GET", sizeof("_GET") - 1))) {
				ZVAL_DEREF(data);
				if (Z_TYPE_P(data) == IS_ARRAY && (ppid = zend_hash_str_find(Z_ARRVAL_P(data), PS(session_name), lensess))) {
					ppid2sid(ppid);
				}
			}
			if (!PS(id) && (data = zend_hash_str_find(&EG(symbol_table), "_POST", sizeof("_POST") - 1))) {
				ZVAL_DEREF(data);
				if (Z_TYPE_P(data) == IS_ARRAY && (ppid = zend_hash_str_find(Z_ARRVAL_P(data), PS(session_name), lensess))) {
					ppid2sid(ppid);
				}
			}
			/* Check the REQUEST_URI symbol for a string of the form
			 * '<session-name>=<session-id>' to allow URLs of the form
			 * http://yoursite/<session-name>=<session-id>/script.php */
			if (!PS(id) && zend_is_auto_global_str("_SERVER", sizeof("_SERVER") - 1) == SUCCESS &&
				(data = zend_hash_str_find(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]), "REQUEST_URI", sizeof("REQUEST_URI") - 1)) &&
				Z_TYPE_P(data) == IS_STRING &&
				(p = strstr(Z_STRVAL_P(data), PS(session_name))) &&
				p[lensess] == '='
				) {
				char *q;
				p += lensess + 1;
				if ((q = strpbrk(p, "/?\\"))) {
					PS(id) = zend_string_init(p, q - p, 0);
				}
			}
			/* Check whether the current request was referred to by
			 * an external site which invalidates the previously found id. */
			if (PS(id) && PS(extern_referer_chk)[0] != '\0' &&
				!Z_ISUNDEF(PG(http_globals)[TRACK_VARS_SERVER]) &&
				(data = zend_hash_str_find(Z_ARRVAL(PG(http_globals)[TRACK_VARS_SERVER]), "HTTP_REFERER", sizeof("HTTP_REFERER") - 1)) &&
				Z_TYPE_P(data) == IS_STRING &&
				Z_STRLEN_P(data) != 0 &&
				strstr(Z_STRVAL_P(data), PS(extern_referer_chk)) == NULL
			) {
				zend_string_release(PS(id));
				PS(id) = NULL;
			}
		}
	}

	/* Finally check session id for dangerous characters
	 * Security note: session id may be embedded in HTML pages.*/
	if (PS(id) && strpbrk(ZSTR_VAL(PS(id)), "\r\n\t <>'\"\\")) {
		zend_string_release(PS(id));
		PS(id) = NULL;
	}

	if (php_session_initialize() == FAILURE
		|| php_session_cache_limiter() == -2) {
		PS(session_status) = php_session_none;
		if (PS(id)) {
			zend_string_release(PS(id));
			PS(id) = NULL;
		}
		return FAILURE;
	}
	return SUCCESS;
}