PHPAPI int php_session_reset_id(void) /* {{{ */
{
	int module_number = PS(module_number);
	zval *sid, *data, *ppid;
	zend_bool apply_trans_sid;

	if (!PS(id)) {
		php_error_docref(NULL, E_WARNING, "Cannot set session ID - session ID is not initialized");
		return FAILURE;
	}

	if (PS(use_cookies) && PS(send_cookie)) {
		php_session_send_cookie();
		PS(send_cookie) = 0;
	}

	/* If the SID constant exists, destroy it. */
	/* We must not delete any items in EG(zend_contants) */
	/* zend_hash_str_del(EG(zend_constants), "sid", sizeof("sid") - 1); */
	sid = zend_get_constant_str("SID", sizeof("SID") - 1);

	if (PS(define_sid)) {
		smart_str var = {0};

		smart_str_appends(&var, PS(session_name));
		smart_str_appendc(&var, '=');
		smart_str_appends(&var, ZSTR_VAL(PS(id)));
		smart_str_0(&var);
		if (sid) {
			zend_string_release(Z_STR_P(sid));
			ZVAL_NEW_STR(sid, var.s);
		} else {
			REGISTER_STRINGL_CONSTANT("SID", ZSTR_VAL(var.s), ZSTR_LEN(var.s), 0);
			smart_str_free(&var);
		}
	} else {
		if (sid) {
			zend_string_release(Z_STR_P(sid));
			ZVAL_EMPTY_STRING(sid);
		} else {
			REGISTER_STRINGL_CONSTANT("SID", "", 0, 0);
		}
	}

	/* Apply trans sid if sid cookie is not set */
	apply_trans_sid = 0;
	if (APPLY_TRANS_SID) {
		apply_trans_sid = 1;
		if (PS(use_cookies) &&
			(data = zend_hash_str_find(&EG(symbol_table), "_COOKIE", sizeof("_COOKIE") - 1))) {
			ZVAL_DEREF(data);
			if (Z_TYPE_P(data) == IS_ARRAY &&
				(ppid = zend_hash_str_find(Z_ARRVAL_P(data), PS(session_name), strlen(PS(session_name))))) {
				ZVAL_DEREF(ppid);
				apply_trans_sid = 0;
			}
		}
	}
	if (apply_trans_sid) {
		zend_string *sname;
		sname = zend_string_init(PS(session_name), strlen(PS(session_name)), 0);
		php_url_scanner_reset_session_var(sname, 1); /* This may fail when session name has changed */
		zend_string_release(sname);
		php_url_scanner_add_session_var(PS(session_name), strlen(PS(session_name)), ZSTR_VAL(PS(id)), ZSTR_LEN(PS(id)), 1);
	}
	return SUCCESS;
}