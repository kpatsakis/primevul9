_php_mb_regex_ereg_search_exec(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	size_t argc = ZEND_NUM_ARGS();
	char *arg_pattern, *arg_options;
	int arg_pattern_len, arg_options_len;
	int n, i, err, pos, len, beg, end;
	OnigOptionType option;
	OnigUChar *str;
	OnigSyntaxType *syntax;

	if (zend_parse_parameters(argc TSRMLS_CC, "|ss", &arg_pattern, &arg_pattern_len, &arg_options, &arg_options_len) == FAILURE) {
		return;
	}

	option = MBREX(regex_default_options);

	if (argc == 2) {
		option = 0;
		_php_mb_regex_init_options(arg_options, arg_options_len, &option, &syntax, NULL);
	}

	if (argc > 0) {
		/* create regex pattern buffer */
		if ((MBREX(search_re) = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, option, MBREX(current_mbctype), MBREX(regex_default_syntax) TSRMLS_CC)) == NULL) {
			RETURN_FALSE;
		}
	}

	pos = MBREX(search_pos);
	str = NULL;
	len = 0;
	if (MBREX(search_str) != NULL && Z_TYPE_P(MBREX(search_str)) == IS_STRING){
		str = (OnigUChar *)Z_STRVAL_P(MBREX(search_str));
		len = Z_STRLEN_P(MBREX(search_str));
	}

	if (MBREX(search_re) == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No regex given");
		RETURN_FALSE;
	}

	if (str == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "No string given");
		RETURN_FALSE;
	}

	if (MBREX(search_regs)) {
		onig_region_free(MBREX(search_regs), 1);
	}
	MBREX(search_regs) = onig_region_new();

	err = onig_search(MBREX(search_re), str, str + len, str + pos, str  + len, MBREX(search_regs), 0);
	if (err == ONIG_MISMATCH) {
		MBREX(search_pos) = len;
		RETVAL_FALSE;
	} else if (err <= -2) {
		OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];
		onig_error_code_to_str(err_str, err);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "mbregex search failure in mbregex_search(): %s", err_str);
		RETVAL_FALSE;
	} else {
		if (MBREX(search_regs)->beg[0] == MBREX(search_regs)->end[0]) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty regular expression");
		}
		switch (mode) {
		case 1:
			array_init(return_value);
			beg = MBREX(search_regs)->beg[0];
			end = MBREX(search_regs)->end[0];
			add_next_index_long(return_value, beg);
			add_next_index_long(return_value, end - beg);
			break;
		case 2:
			array_init(return_value);
			n = MBREX(search_regs)->num_regs;
			for (i = 0; i < n; i++) {
				beg = MBREX(search_regs)->beg[i];
				end = MBREX(search_regs)->end[i];
				if (beg >= 0 && beg <= end && end <= len) {
					add_index_stringl(return_value, i, (char *)&str[beg], end - beg, 1);
				} else {
					add_index_bool(return_value, i, 0);
				}
			}
			break;
		default:
			RETVAL_TRUE;
			break;
		}
		end = MBREX(search_regs)->end[0];
		if (pos < end) {
			MBREX(search_pos) = end;
		} else {
			MBREX(search_pos) = pos + 1;
		}
	}

	if (err < 0) {
		onig_region_free(MBREX(search_regs), 1);
		MBREX(search_regs) = (OnigRegion *)NULL;
	}
}