static PHP_INI_MH(OnUpdateRfc1867Freq) /* {{{ */
{
	int tmp;
	tmp = zend_atoi(ZSTR_VAL(new_value), (int)ZSTR_LEN(new_value));
	if(tmp < 0) {
		php_error_docref(NULL, E_WARNING, "session.upload_progress.freq must be greater than or equal to zero");
		return FAILURE;
	}
	if(ZSTR_LEN(new_value) > 0 && ZSTR_VAL(new_value)[ZSTR_LEN(new_value)-1] == '%') {
		if(tmp > 100) {
			php_error_docref(NULL, E_WARNING, "session.upload_progress.freq cannot be over 100%%");
			return FAILURE;
		}
		PS(rfc1867_freq) = -tmp;
	} else {
		PS(rfc1867_freq) = tmp;
	}
	return SUCCESS;
} /* }}} */