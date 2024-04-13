static void _php_mb_regex_ereg_exec(INTERNAL_FUNCTION_PARAMETERS, int icase)
{
	zval *arg_pattern, *array = NULL;
	char *string;
	size_t string_len;
	php_mb_regex_t *re;
	OnigRegion *regs = NULL;
	int i, match_len, beg, end;
	OnigOptionType options;
	char *str;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zs|z/", &arg_pattern, &string, &string_len, &array) == FAILURE) {
		RETURN_FALSE;
	}

	if (array != NULL) {
		zval_ptr_dtor(array);
		array_init(array);
	}

	if (!php_mb_check_encoding(
		string,
		string_len,
		_php_mb_regex_mbctype2name(MBREX(current_mbctype))
	)) {
		RETURN_FALSE;
	}

	options = MBREX(regex_default_options);
	if (icase) {
		options |= ONIG_OPTION_IGNORECASE;
	}

	/* compile the regular expression from the supplied regex */
	if (Z_TYPE_P(arg_pattern) != IS_STRING) {
		/* we convert numbers to integers and treat them as a string */
		if (Z_TYPE_P(arg_pattern) == IS_DOUBLE) {
			convert_to_long_ex(arg_pattern);	/* get rid of decimal places */
		}
		convert_to_string_ex(arg_pattern);
		/* don't bother doing an extended regex with just a number */
	}

	if (Z_STRLEN_P(arg_pattern) == 0) {
		php_error_docref(NULL, E_WARNING, "empty pattern");
		RETVAL_FALSE;
		goto out;
	}

	re = php_mbregex_compile_pattern(Z_STRVAL_P(arg_pattern), Z_STRLEN_P(arg_pattern), options, MBREX(current_mbctype), MBREX(regex_default_syntax));
	if (re == NULL) {
		RETVAL_FALSE;
		goto out;
	}

	regs = onig_region_new();

	/* actually execute the regular expression */
	if (onig_search(re, (OnigUChar *)string, (OnigUChar *)(string + string_len), (OnigUChar *)string, (OnigUChar *)(string + string_len), regs, 0) < 0) {
		RETVAL_FALSE;
		goto out;
	}

	match_len = 1;
	str = string;
	if (array != NULL) {

		match_len = regs->end[0] - regs->beg[0];
		for (i = 0; i < regs->num_regs; i++) {
			beg = regs->beg[i];
			end = regs->end[i];
			if (beg >= 0 && beg < end && (size_t)end <= string_len) {
				add_index_stringl(array, i, (char *)&str[beg], end - beg);
			} else {
				add_index_bool(array, i, 0);
			}
		}

		if (onig_number_of_names(re) > 0) {
			mb_regex_groups_iter_args args = {array, string, string_len, regs};
			onig_foreach_name(re, mb_regex_groups_iter, &args);
		}
	}

	if (match_len == 0) {
		match_len = 1;
	}
	RETVAL_LONG(match_len);
out:
	if (regs != NULL) {
		onig_region_free(regs, 1);
	}
}