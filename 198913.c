PHP_FUNCTION(mb_ereg_match)
{
	char *arg_pattern;
	int arg_pattern_len;

	char *string;
	int string_len;

	php_mb_regex_t *re;
	OnigSyntaxType *syntax;
	OnigOptionType option = 0;
	int err;

	{
		char *option_str = NULL;
		int option_str_len = 0;

		if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|s",
		                          &arg_pattern, &arg_pattern_len, &string, &string_len,
		                          &option_str, &option_str_len)==FAILURE) {
			RETURN_FALSE;
		}

		if (option_str != NULL) {
			_php_mb_regex_init_options(option_str, option_str_len, &option, &syntax, NULL);
		} else {
			option |= MBREX(regex_default_options);
			syntax = MBREX(regex_default_syntax);
		}
	}

	if ((re = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, option, MBREX(current_mbctype), syntax TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}

	/* match */
	err = onig_match(re, (OnigUChar *)string, (OnigUChar *)(string + string_len), (OnigUChar *)string, NULL, 0);
	if (err >= 0) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}