static zend_bool php_check_cancel_upload(php_session_rfc1867_progress *progress) /* {{{ */
{
	zval *progress_ary, *cancel_upload;

	if ((progress_ary = zend_symtable_find(Z_ARRVAL_P(Z_REFVAL(PS(http_session_vars))), progress->key.s)) == NULL) {
		return 0;
	}
	if (Z_TYPE_P(progress_ary) != IS_ARRAY) {
		return 0;
	}
	if ((cancel_upload = zend_hash_str_find(Z_ARRVAL_P(progress_ary), "cancel_upload", sizeof("cancel_upload") - 1)) == NULL) {
		return 0;
	}
	return Z_TYPE_P(cancel_upload) == IS_TRUE;
} /* }}} */