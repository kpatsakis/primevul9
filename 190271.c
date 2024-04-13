PHP_FUNCTION(iconv_mime_decode)
{
	char *encoded_str;
	int encoded_str_len;
	char *charset = get_internal_encoding(TSRMLS_C);
	int charset_len = 0;
	long mode = 0;

	smart_str retval = {0};

	php_iconv_err_t err;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls",
		&encoded_str, &encoded_str_len, &mode, &charset, &charset_len) == FAILURE) {

		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	err = _php_iconv_mime_decode(&retval, encoded_str, encoded_str_len, charset, NULL, mode);
	_php_iconv_show_error(err, charset, "???" TSRMLS_CC);

	if (err == PHP_ICONV_ERR_SUCCESS) {
		if (retval.c != NULL) {
			RETVAL_STRINGL(retval.c, retval.len, 0);
		} else {
			RETVAL_EMPTY_STRING();
		}
	} else {
		smart_str_free(&retval);
		RETVAL_FALSE;
	}
}