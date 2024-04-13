static void _php_mb_regex_ereg_replace_exec(INTERNAL_FUNCTION_PARAMETERS, OnigOptionType options, int is_callable)
{
	zval *arg_pattern_zval;

	char *arg_pattern;
	size_t arg_pattern_len;

	char *replace;
	size_t replace_len;

	zend_fcall_info arg_replace_fci;
	zend_fcall_info_cache arg_replace_fci_cache;

	char *string;
	size_t string_len;

	php_mb_regex_t *re;
	OnigSyntaxType *syntax;
	OnigRegion *regs = NULL;
	smart_str out_buf = {0};
	smart_str eval_buf = {0};
	smart_str *pbuf;
	int err, eval, n;
	OnigUChar *pos;
	OnigUChar *string_lim;
	char *description = NULL;
	char pat_buf[6];

	const mbfl_encoding *enc;

	{
		const char *current_enc_name;
		current_enc_name = _php_mb_regex_mbctype2name(MBREX(current_mbctype));
		if (current_enc_name == NULL ||
			(enc = mbfl_name2encoding(current_enc_name)) == NULL) {
			php_error_docref(NULL, E_WARNING, "Unknown error");
			RETURN_FALSE;
		}
	}
	eval = 0;
	{
		char *option_str = NULL;
		size_t option_str_len = 0;

		if (!is_callable) {
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "zss|s",
						&arg_pattern_zval,
						&replace, &replace_len,
						&string, &string_len,
						&option_str, &option_str_len) == FAILURE) {
				RETURN_FALSE;
			}
		} else {
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "zfs|s",
						&arg_pattern_zval,
						&arg_replace_fci, &arg_replace_fci_cache,
						&string, &string_len,
						&option_str, &option_str_len) == FAILURE) {
				RETURN_FALSE;
			}
		}

		if (!php_mb_check_encoding(
		string,
		string_len,
		_php_mb_regex_mbctype2name(MBREX(current_mbctype))
		)) {
			RETURN_NULL();
		}

		if (option_str != NULL) {
			_php_mb_regex_init_options(option_str, option_str_len, &options, &syntax, &eval);
		} else {
			options |= MBREX(regex_default_options);
			syntax = MBREX(regex_default_syntax);
		}
	}
	if (eval && !is_callable) {
		php_error_docref(NULL, E_DEPRECATED, "The 'e' option is deprecated, use mb_ereg_replace_callback instead");
	}
	if (Z_TYPE_P(arg_pattern_zval) == IS_STRING) {
		arg_pattern = Z_STRVAL_P(arg_pattern_zval);
		arg_pattern_len = Z_STRLEN_P(arg_pattern_zval);
	} else {
		/* FIXME: this code is not multibyte aware! */
		convert_to_long_ex(arg_pattern_zval);
		pat_buf[0] = (char)Z_LVAL_P(arg_pattern_zval);
		pat_buf[1] = '\0';
		pat_buf[2] = '\0';
		pat_buf[3] = '\0';
		pat_buf[4] = '\0';
		pat_buf[5] = '\0';

		arg_pattern = pat_buf;
		arg_pattern_len = 1;
	}
	/* create regex pattern buffer */
	re = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, options, MBREX(current_mbctype), syntax);
	if (re == NULL) {
		RETURN_FALSE;
	}

	if (eval || is_callable) {
		pbuf = &eval_buf;
		description = zend_make_compiled_string_description("mbregex replace");
	} else {
		pbuf = &out_buf;
		description = NULL;
	}

	if (is_callable) {
		if (eval) {
			php_error_docref(NULL, E_WARNING, "Option 'e' cannot be used with replacement callback");
			RETURN_FALSE;
		}
	}

	/* do the actual work */
	err = 0;
	pos = (OnigUChar *)string;
	string_lim = (OnigUChar*)(string + string_len);
	regs = onig_region_new();
	while (err >= 0) {
		err = onig_search(re, (OnigUChar *)string, (OnigUChar *)string_lim, pos, (OnigUChar *)string_lim, regs, 0);
		if (err <= -2) {
			OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];
			onig_error_code_to_str(err_str, err);
			php_error_docref(NULL, E_WARNING, "mbregex search failure in php_mbereg_replace_exec(): %s", err_str);
			break;
		}
		if (err >= 0) {
			/* copy the part of the string before the match */
			smart_str_appendl(&out_buf, (char *)pos, (size_t)((OnigUChar *)(string + regs->beg[0]) - pos));

			if (!is_callable) {
				mb_regex_substitute(pbuf, string, string_len, replace, replace_len, re, regs, enc);
			}

			if (eval) {
				zval v;
				zend_string *eval_str;
				/* null terminate buffer */
				smart_str_0(&eval_buf);

				if (eval_buf.s) {
					eval_str = eval_buf.s;
				} else {
					eval_str = ZSTR_EMPTY_ALLOC();
				}

				/* do eval */
				if (zend_eval_stringl(ZSTR_VAL(eval_str), ZSTR_LEN(eval_str), &v, description) == FAILURE) {
					efree(description);
					zend_throw_error(NULL, "Failed evaluating code: %s%s", PHP_EOL, ZSTR_VAL(eval_str));
					onig_region_free(regs, 0);
					smart_str_free(&out_buf);
					smart_str_free(&eval_buf);
					RETURN_FALSE;
				}

				/* result of eval */
				convert_to_string(&v);
				smart_str_appendl(&out_buf, Z_STRVAL(v), Z_STRLEN(v));
				/* Clean up */
				smart_str_free(&eval_buf);
				zval_ptr_dtor_str(&v);
			} else if (is_callable) {
				zval args[1];
				zval subpats, retval;
				int i;

				array_init(&subpats);
				for (i = 0; i < regs->num_regs; i++) {
					add_next_index_stringl(&subpats, string + regs->beg[i], regs->end[i] - regs->beg[i]);
				}
				if (onig_number_of_names(re) > 0) {
					mb_regex_groups_iter_args args = {&subpats, string, string_len, regs};
					onig_foreach_name(re, mb_regex_groups_iter, &args);
				}

				ZVAL_COPY_VALUE(&args[0], &subpats);
				/* null terminate buffer */
				smart_str_0(&eval_buf);

				arg_replace_fci.param_count = 1;
				arg_replace_fci.params = args;
				arg_replace_fci.retval = &retval;
				if (zend_call_function(&arg_replace_fci, &arg_replace_fci_cache) == SUCCESS &&
						!Z_ISUNDEF(retval)) {
					convert_to_string_ex(&retval);
					smart_str_appendl(&out_buf, Z_STRVAL(retval), Z_STRLEN(retval));
					smart_str_free(&eval_buf);
					zval_ptr_dtor(&retval);
				} else {
					if (!EG(exception)) {
						php_error_docref(NULL, E_WARNING, "Unable to call custom replacement function");
					}
				}
				zval_ptr_dtor(&subpats);
			}

			n = regs->end[0];
			if ((pos - (OnigUChar *)string) < n) {
				pos = (OnigUChar *)string + n;
			} else {
				if (pos < string_lim) {
					smart_str_appendl(&out_buf, (char *)pos, 1);
				}
				pos++;
			}
		} else { /* nomatch */
			/* stick that last bit of string on our output */
			if (string_lim - pos > 0) {
				smart_str_appendl(&out_buf, (char *)pos, string_lim - pos);
			}
		}
		onig_region_free(regs, 0);
	}

	if (description) {
		efree(description);
	}
	if (regs != NULL) {
		onig_region_free(regs, 1);
	}
	smart_str_free(&eval_buf);

	if (err <= -2) {
		smart_str_free(&out_buf);
		RETVAL_FALSE;
	} else if (out_buf.s) {
		smart_str_0(&out_buf);
		RETVAL_STR(out_buf.s);
	} else {
		RETVAL_EMPTY_STRING();
	}
}