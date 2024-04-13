static inline unsigned php_unicode_tofold_simple(unsigned code, enum mbfl_no_encoding enc) {
	code = php_unicode_tofold_raw(code, enc);
	if (UNEXPECTED(code > 0xffffff)) {
		return _uccase_extra_table[code & 0xffffff];
	}
	return code;
}