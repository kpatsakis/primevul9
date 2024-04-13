PHP_FUNCTION(iconv_strpos)
{
	char *charset = get_internal_encoding(TSRMLS_C);
	int charset_len = 0;
	char *haystk;
	int haystk_len;
	char *ndl;
	int ndl_len;
	long offset = 0;

	php_iconv_err_t err;

	unsigned int retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|ls",
		&haystk, &haystk_len, &ndl, &ndl_len,
		&offset, &charset, &charset_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	if (offset < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Offset not contained in string.");
		RETURN_FALSE;
	}

	if (ndl_len < 1) {
		RETURN_FALSE;
	}

	err = _php_iconv_strpos(&retval, haystk, haystk_len, ndl, ndl_len,
	                        offset, charset);
	_php_iconv_show_error(err, GENERIC_SUPERSET_NAME, charset TSRMLS_CC);

	if (err == PHP_ICONV_ERR_SUCCESS && retval != (unsigned int)-1) {
		RETVAL_LONG((long)retval);
	} else {
		RETVAL_FALSE;
	}
}