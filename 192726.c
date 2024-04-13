PHPAPI long _php_math_basetolong(zval *arg, int base)
{
	long num = 0, digit, onum;
	int i;
	char c, *s;

	if (Z_TYPE_P(arg) != IS_STRING || base < 2 || base > 36) {
		return 0;
	}

	s = Z_STRVAL_P(arg);

	for (i = Z_STRLEN_P(arg); i > 0; i--) {
		c = *s++;
		
		digit = (c >= '0' && c <= '9') ? c - '0'
			: (c >= 'A' && c <= 'Z') ? c - 'A' + 10
			: (c >= 'a' && c <= 'z') ? c - 'a' + 10
			: base;
		
		if (digit >= base) {
			continue;
		}

		onum = num;
		num = num * base + digit;
		if (num > onum)
			continue;

		{
			TSRMLS_FETCH();

			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number '%s' is too big to fit in long", s);
			return LONG_MAX;
		}
	}

	return num;
}