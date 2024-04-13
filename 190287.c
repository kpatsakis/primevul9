PHP_FUNCTION(iconv_strlen)
{
	char *charset = get_internal_encoding(TSRMLS_C);
	int charset_len = 0;
	char *str;
	int str_len;

	php_iconv_err_t err;

	unsigned int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s",
		&str, &str_len, &charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	err = _php_iconv_strlen(&retval, str, str_len, charset);
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset TSRMLS_CC);
	if (err == PHP_ICONV_ERR_SUCCESS) {
		RETVAL_LONG(retval);
	} else {
		RETVAL_FALSE;
	}
}