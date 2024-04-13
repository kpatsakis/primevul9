static unsigned php_unicode_totitle_raw(unsigned code, enum mbfl_no_encoding enc)
{
	unsigned new_code = CASE_LOOKUP(code, title);
	if (new_code != CODE_NOT_FOUND) {
		return new_code;
	}

	/* No dedicated title-case variant, use to-upper instead */
	return php_unicode_toupper_raw(code, enc);
}