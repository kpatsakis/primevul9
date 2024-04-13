PHP_FUNCTION(mb_ereg_search_init)
{
	int argc = ZEND_NUM_ARGS();
	zend_string *arg_str;
	char *arg_pattern = NULL, *arg_options = NULL;
	size_t arg_pattern_len = 0, arg_options_len = 0;
	OnigSyntaxType *syntax = NULL;
	OnigOptionType option;

	if (zend_parse_parameters(argc, "S|ss", &arg_str, &arg_pattern, &arg_pattern_len, &arg_options, &arg_options_len) == FAILURE) {
		return;
	}

	if (argc > 1 && arg_pattern_len == 0) {
		php_error_docref(NULL, E_WARNING, "Empty pattern");
		RETURN_FALSE;
	}

	option = MBREX(regex_default_options);
	syntax = MBREX(regex_default_syntax);

	if (argc == 3) {
		option = 0;
		_php_mb_regex_init_options(arg_options, arg_options_len, &option, &syntax, NULL);
	}

	if (argc > 1) {
		/* create regex pattern buffer */
		if ((MBREX(search_re) = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, option, MBREX(current_mbctype), syntax)) == NULL) {
			RETURN_FALSE;
		}
	}

	if (!Z_ISNULL(MBREX(search_str))) {
		zval_ptr_dtor(&MBREX(search_str));
	}

	ZVAL_STR_COPY(&MBREX(search_str), arg_str);

	if (php_mb_check_encoding(
	ZSTR_VAL(arg_str),
	ZSTR_LEN(arg_str),
	_php_mb_regex_mbctype2name(MBREX(current_mbctype))
	)) {
		MBREX(search_pos) = 0;
		RETVAL_TRUE;
	} else {
		MBREX(search_pos) = ZSTR_LEN(arg_str);
		RETVAL_FALSE;
	}

	if (MBREX(search_regs) != NULL) {
		onig_region_free(MBREX(search_regs), 1);
		MBREX(search_regs) = NULL;
	}
}