PHP_NAMED_FUNCTION(php_if_iconv)
{
	char *in_charset, *out_charset, *in_buffer, *out_buffer;
	size_t out_len;
	int in_charset_len = 0, out_charset_len = 0, in_buffer_len;
	php_iconv_err_t err;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss",
		&in_charset, &in_charset_len, &out_charset, &out_charset_len, &in_buffer, &in_buffer_len) == FAILURE)
		return;

	if (in_charset_len >= ICONV_CSNMAXLEN || out_charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	err = php_iconv_string(in_buffer, (size_t)in_buffer_len,
		&out_buffer, &out_len, out_charset, in_charset);
	_php_iconv_show_error(err, out_charset, in_charset TSRMLS_CC);
	if (err == PHP_ICONV_ERR_SUCCESS && out_buffer != NULL) {
		RETVAL_STRINGL_CHECK(out_buffer, out_len, 0);
	} else {
		if (out_buffer != NULL) {
			efree(out_buffer);
		}
		RETURN_FALSE;
	}
}