PHP_FUNCTION(iconv_substr)
{
	char *charset = get_internal_encoding(TSRMLS_C);
	int charset_len = 0;
	char *str;
	int str_len;
	long offset, length = 0;

	php_iconv_err_t err;

	smart_str retval = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|ls",
		&str, &str_len, &offset, &length,
		&charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	if (ZEND_NUM_ARGS() < 3) {
		length = str_len;
	}

	err = _php_iconv_substr(&retval, str, str_len, offset, length, charset);
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset TSRMLS_CC);

	if (err == PHP_ICONV_ERR_SUCCESS && str != NULL && retval.c != NULL) {
		RETURN_STRINGL(retval.c, retval.len, 0);
	}
	smart_str_free(&retval);
	RETURN_FALSE;
}