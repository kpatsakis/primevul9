static unsigned php_unicode_tolower_raw(unsigned code, enum mbfl_no_encoding enc)
{
	if (code < 0x80) {
		/* Fast path for ASCII */
		if (code >= 0x41 && code <= 0x5A) {
			if (UNEXPECTED(enc == mbfl_no_encoding_8859_9 && code == 0x0049L)) {
				return 0x0131L;
			}
			return code + 0x20;
		}
		return code;
	} else {
		unsigned new_code = CASE_LOOKUP(code, lower);
		if (new_code != CODE_NOT_FOUND) {
			if (UNEXPECTED(enc == mbfl_no_encoding_8859_9 && code == 0x130)) {
				return 0x69;
			}
			return new_code;
		}
		return code;
	}
}