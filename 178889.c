PHPAPI char *php_url_encode(char const *s, int len, int *new_length)
{
	register unsigned char c;
	unsigned char *to, *start;
	unsigned char const *from, *end;

	from = (unsigned char *)s;
	end = (unsigned char *)s + len;
	start = to = (unsigned char *) safe_emalloc(3, len, 1);

	while (from < end) {
		c = *from++;

		if (c == ' ') {
			*to++ = '+';
#ifndef CHARSET_EBCDIC
		} else if ((c < '0' && c != '-' && c != '.') ||
				   (c < 'A' && c > '9') ||
				   (c > 'Z' && c < 'a' && c != '_') ||
				   (c > 'z')) {
			to[0] = '%';
			to[1] = hexchars[c >> 4];
			to[2] = hexchars[c & 15];
			to += 3;
#else /*CHARSET_EBCDIC*/
		} else if (!isalnum(c) && strchr("_-.", c) == NULL) {
			/* Allow only alphanumeric chars and '_', '-', '.'; escape the rest */
			to[0] = '%';
			to[1] = hexchars[os_toascii[c] >> 4];
			to[2] = hexchars[os_toascii[c] & 15];
			to += 3;
#endif /*CHARSET_EBCDIC*/
		} else {
			*to++ = c;
		}
	}

	if ((to-start) > INT_MAX) {
		TSRMLS_FETCH();
		/* E_ERROR since most clients won't check for error, and this is rather rare condition */
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "String overflow, max length is %d", INT_MAX);
	}

	*to = 0;
	if (new_length) {
		*new_length = to - start;
	}
	return (char *) start;
}