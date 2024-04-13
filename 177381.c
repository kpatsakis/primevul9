PHP_FUNCTION(mb_ereg_search_getregs)
{
	size_t n, i, len, beg, end;
	OnigUChar *str;

	if (MBREX(search_regs) != NULL && Z_TYPE(MBREX(search_str)) == IS_STRING) {
		array_init(return_value);

		str = (OnigUChar *)Z_STRVAL(MBREX(search_str));
		len = Z_STRLEN(MBREX(search_str));
		n = MBREX(search_regs)->num_regs;
		for (i = 0; i < n; i++) {
			beg = MBREX(search_regs)->beg[i];
			end = MBREX(search_regs)->end[i];
			if (beg >= 0 && beg <= end && end <= len) {
				add_index_stringl(return_value, i, (char *)&str[beg], end - beg);
			} else {
				add_index_bool(return_value, i, 0);
			}
		}
		if (onig_number_of_names(MBREX(search_re)) > 0) {
			mb_regex_groups_iter_args args = {
				return_value,
				Z_STRVAL(MBREX(search_str)),
				len,
				MBREX(search_regs)
			};
			onig_foreach_name(MBREX(search_re), mb_regex_groups_iter, &args);
		}
	} else {
		RETVAL_FALSE;
	}
}