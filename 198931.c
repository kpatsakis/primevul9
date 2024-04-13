PHP_FUNCTION(mb_regex_set_options)
{
	OnigOptionType opt;
	OnigSyntaxType *syntax;
	char *string = NULL;
	int string_len;
	char buf[16];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s",
	                          &string, &string_len) == FAILURE) {
		RETURN_FALSE;
	}
	if (string != NULL) {
		opt = 0;
		syntax = NULL;
		_php_mb_regex_init_options(string, string_len, &opt, &syntax, NULL);
		_php_mb_regex_set_options(opt, syntax, NULL, NULL TSRMLS_CC);
	} else {
		opt = MBREX(regex_default_options);
		syntax = MBREX(regex_default_syntax);
	}
	_php_mb_regex_get_option_string(buf, sizeof(buf), opt, syntax);

	RETVAL_STRING(buf, 1);
}