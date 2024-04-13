static unsigned php_unicode_toupper_raw(unsigned code, enum mbfl_no_encoding enc)
{
	if (code < 0x80) {
		/* Fast path for ASCII */
		if (code >= 0x61 && code <= 0x7A) {
			if (UNEXPECTED(enc == mbfl_no_encoding_8859_9 && code == 0x69)) {
				return 0x130;
			}
			return code - 0x20;
		}
		return code;
	} else {
		unsigned new_code = CASE_LOOKUP(code, upper);
		if (new_code != CODE_NOT_FOUND) {
			return new_code;
		}
		return code;
	}
}