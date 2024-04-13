PHPAPI char * _php_math_longtobase(zval *arg, int base)
{
	static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char buf[(sizeof(unsigned long) << 3) + 1];
	char *ptr, *end;
	unsigned long value;

	if (Z_TYPE_P(arg) != IS_LONG || base < 2 || base > 36) {
		return STR_EMPTY_ALLOC();
	}

	value = Z_LVAL_P(arg);

	end = ptr = buf + sizeof(buf) - 1;
	*ptr = '\0';

	do {
		*--ptr = digits[value % base];
		value /= base;
	} while (ptr > buf && value);

	return estrndup(ptr, end - ptr);
}