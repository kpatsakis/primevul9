PHP_FUNCTION(mb_ereg_search_getregs)
{
	int n, i, len, beg, end;
	OnigUChar *str;

	if (MBREX(search_regs) != NULL && Z_TYPE_P(MBREX(search_str)) == IS_STRING && Z_STRVAL_P(MBREX(search_str)) != NULL) {
		array_init(return_value);

		str = (OnigUChar *)Z_STRVAL_P(MBREX(search_str));
		len = Z_STRLEN_P(MBREX(search_str));
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
	} else {
		RETVAL_FALSE;
	}
}