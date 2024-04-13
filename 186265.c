static void ppid2sid(zval *ppid) {
	ZVAL_DEREF(ppid);
	if (Z_TYPE_P(ppid) == IS_STRING) {
		PS(id) = zend_string_init(Z_STRVAL_P(ppid), Z_STRLEN_P(ppid), 0);
		PS(send_cookie) = 0;
	} else {
		PS(id) = NULL;
		PS(send_cookie) = 1;
	}
}