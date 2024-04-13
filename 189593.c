static inline unsigned php_unicode_tolower_full(
		unsigned code, enum mbfl_no_encoding enc, unsigned *out) {
	code = php_unicode_tolower_raw(code, enc);
	if (UNEXPECTED(code > 0xffffff)) {
		unsigned len = code >> 24;
		const unsigned *p = &_uccase_extra_table[code & 0xffffff];
		memcpy(out, p + 1, len * sizeof(unsigned));
		return len;
	}
	*out = code;
	return 1;
}