PHP_FUNCTION(iconv_set_encoding)
{
	char *type, *charset;
	int type_len, charset_len =0, retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &type, &type_len, &charset, &charset_len) == FAILURE)
		return;

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	if(!strcasecmp("input_encoding", type)) {
		retval = zend_alter_ini_entry("iconv.input_encoding", sizeof("iconv.input_encoding"), charset, charset_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	} else if(!strcasecmp("output_encoding", type)) {
		retval = zend_alter_ini_entry("iconv.output_encoding", sizeof("iconv.output_encoding"), charset, charset_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	} else if(!strcasecmp("internal_encoding", type)) {
		retval = zend_alter_ini_entry("iconv.internal_encoding", sizeof("iconv.internal_encoding"), charset, charset_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	} else {
		RETURN_FALSE;
	}

	if (retval == SUCCESS) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}