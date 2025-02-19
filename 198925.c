PHP_FUNCTION(mb_split)
{
	char *arg_pattern;
	int arg_pattern_len;
	php_mb_regex_t *re;
	OnigRegion *regs = NULL;
	char *string;
	OnigUChar *pos, *chunk_pos;
	int string_len;

	int n, err;
	long count = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", &arg_pattern, &arg_pattern_len, &string, &string_len, &count) == FAILURE) {
		RETURN_FALSE;
	}

	if (count > 0) {
		count--;
	}

	/* create regex pattern buffer */
	if ((re = php_mbregex_compile_pattern(arg_pattern, arg_pattern_len, MBREX(regex_default_options), MBREX(current_mbctype), MBREX(regex_default_syntax) TSRMLS_CC)) == NULL) {
		RETURN_FALSE;
	}

	array_init(return_value);

	chunk_pos = pos = (OnigUChar *)string;
	err = 0;
	regs = onig_region_new();
	/* churn through str, generating array entries as we go */
	while (count != 0 && (pos - (OnigUChar *)string) < string_len) {
		int beg, end;
		err = onig_search(re, (OnigUChar *)string, (OnigUChar *)(string + string_len), pos, (OnigUChar *)(string + string_len), regs, 0);
		if (err < 0) {
			break;
		}
		beg = regs->beg[0], end = regs->end[0];
		/* add it to the array */
		if ((pos - (OnigUChar *)string) < end) {
			if (beg < string_len && beg >= (chunk_pos - (OnigUChar *)string)) {
				add_next_index_stringl(return_value, (char *)chunk_pos, ((OnigUChar *)(string + beg) - chunk_pos), 1);
				--count;
			} else {
				err = -2;
				break;
			}
			/* point at our new starting point */
			chunk_pos = pos = (OnigUChar *)string + end;
		} else {
			pos++;
		}
		onig_region_free(regs, 0);
	}

	onig_region_free(regs, 1);

	/* see if we encountered an error */
	if (err <= -2) {
		OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];
		onig_error_code_to_str(err_str, err);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "mbregex search failure in mbsplit(): %s", err_str);
		zval_dtor(return_value);
		RETURN_FALSE;
	}

	/* otherwise we just have one last element to add to the array */
	n = ((OnigUChar *)(string + string_len) - chunk_pos);
	if (n > 0) {
		add_next_index_stringl(return_value, (char *)chunk_pos, n, 1);
	} else {
		add_next_index_stringl(return_value, "", 0, 1);
	}
}