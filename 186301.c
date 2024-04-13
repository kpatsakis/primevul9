static zend_bool early_find_sid_in(zval *dest, int where, php_session_rfc1867_progress *progress) /* {{{ */
{
	zval *ppid;

	if (Z_ISUNDEF(PG(http_globals)[where])) {
		return 0;
	}

	if ((ppid = zend_hash_str_find(Z_ARRVAL(PG(http_globals)[where]), PS(session_name), progress->sname_len))
			&& Z_TYPE_P(ppid) == IS_STRING) {
		zval_dtor(dest);
		ZVAL_DEREF(ppid);
		ZVAL_COPY(dest, ppid);
		return 1;
	}

	return 0;
} /* }}} */